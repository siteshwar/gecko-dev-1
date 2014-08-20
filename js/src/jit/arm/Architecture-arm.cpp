/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "jit/arm/Architecture-arm.h"

#ifndef JS_ARM_SIMULATOR
#include <elf.h>
#endif

#include <fcntl.h>
#include <unistd.h>

#include "jit/arm/Assembler-arm.h"
#include "jit/RegisterSets.h"

#if defined(ANDROID) || defined(MOZ_B2G) || defined(JS_ARM_SIMULATOR)
// The Android NDK does not include the hwcap.h kernel header, and it is not
// defined when building the simulator, so inline the header defines we need.
# define HWCAP_VFP        (1 << 6)
# define HWCAP_NEON       (1 << 12)
# define HWCAP_VFPv3      (1 << 13)
# define HWCAP_VFPv3D16   (1 << 14) /* also set for VFPv4-D16 */
# define HWCAP_VFPv4      (1 << 16)
# define HWCAP_IDIVA      (1 << 17)
# define HWCAP_IDIVT      (1 << 18)
# define HWCAP_VFPD32     (1 << 19) /* set if VFP has 32 regs (not 16) */
# define AT_HWCAP 16
#else
# include <asm/hwcap.h>
# if !defined(HWCAP_IDIVA)
#  define HWCAP_IDIVA     (1 << 17)
# endif
# if !defined(HWCAP_VFPD32)
#  define HWCAP_VFPD32    (1 << 19) /* set if VFP has 32 regs (not 16) */
# endif
#endif

// Not part of the HWCAP flag, but we need to know this, and this bit is not
// used so we are using it.
#define HWCAP_ARMv7 (1 << 28)

// Also take a bit to flag the use of the hardfp ABI.
#define HWCAP_USE_HARDFP_ABI (1 << 27)

namespace js {
namespace jit {


// Parse the Linux kernel cpuinfo features. This is also used to parse the
// override features which has some extensions: 'armv7' and 'hardfp'.
uint32_t
ParseARMCpuFeatures(const char *features, bool override = false)
{
    uint32_t flags = 0;

    for (;;) {
        char  ch = *features;
        if (!ch) {
            // End of string.
            break;
        }
        if (ch == ' ' || ch == ',') {
            // Skip separator characters.
            features++;
            continue;
        }
        // Find the end of the token.
        const char *end = features + 1;
        for (; ; end++) {
            ch = *end;
            if (!ch || ch == ' ' || ch == ',')
                break;
        }
        size_t count = end - features;
        if (count == 3 && strncmp(features, "vfp", 3) == 0)
            flags |= HWCAP_VFP;
        else if (count == 4 && strncmp(features, "neon", 4) == 0)
            flags |= HWCAP_NEON;
        else if (count == 5 && strncmp(features, "vfpv3", 5) == 0)
            flags |= HWCAP_VFPv3;
        else if (count == 8 && strncmp(features, "vfpv3d16", 8) == 0)
            flags |= HWCAP_VFPv3D16;
        else if (count == 5 && strncmp(features, "vfpv4", 5) == 0)
            flags |= HWCAP_VFPv4;
        else if (count == 5 && strncmp(features, "idiva", 5) == 0)
            flags |= HWCAP_IDIVA;
        else if (count == 5 && strncmp(features, "idivt", 5) == 0)
            flags |= HWCAP_IDIVT;
        else if (count == 6 && strncmp(features, "vfpd32", 6) == 0)
            flags |= HWCAP_VFPD32;
        else if (count == 5 && strncmp(features, "armv7", 5) == 0)
            flags |= HWCAP_ARMv7;
#if defined(JS_ARM_SIMULATOR)
        else if (count == 6 && strncmp(features, "hardfp", 6) == 0)
            flags |= HWCAP_USE_HARDFP_ABI;
#endif
        else if (override)
            fprintf(stderr, "Warning: unexpected ARM feature at: %s\n", features);
        features = end;
    }
    IonSpew(IonSpew_Codegen, "ARM features: '%s'\n   flags: 0x%x\n", features, flags);
    return flags;
}

// The override flags parsed from the ARMHWCAP environment variable or from the
// --arm-hwcap js shell argument.
volatile static uint32_t armHwCapFlags = 0;

bool
ParseARMHwCapFlags(const char *armHwCap)
{
    uint32_t flags = 0;

    if (!armHwCap || !armHwCap[0])
        return false;

#ifdef JS_CODEGEN_ARM_HARDFP
    flags |= HWCAP_USE_HARDFP_ABI;
#endif

    if (strstr(armHwCap, "help")) {
        fflush(NULL);
        printf(
               "\n"
               "usage: ARMHWCAP=option,option,option,... where options can be:\n"
               "\n"
               "  armv7    \n"
               "  vfp      \n"
               "  neon     \n"
               "  vfpv3    \n"
               "  vfpv3d16 \n"
               "  vfpv4    \n"
               "  idiva    \n"
               "  idivt    \n"
               "  vfpd32   \n"
#if defined(JS_ARM_SIMULATOR)
               "  hardfp   \n"
#endif
               "\n"
               );
        exit(0);
        /*NOTREACHED*/
    }

    armHwCapFlags = ParseARMCpuFeatures(armHwCap, /* override = */ true);
    return true;
}

uint32_t GetARMFlags()
{
    volatile static bool isSet = false;
    volatile static uint32_t flags = 0;
    if (isSet)
        return flags;

    const char *env = getenv("ARMHWCAP");
    if (ParseARMHwCapFlags(env) || armHwCapFlags) {
        flags = armHwCapFlags;
        isSet = true;
        return flags;
    }

#ifdef JS_ARM_SIMULATOR
    flags = HWCAP_ARMv7 | HWCAP_VFP | HWCAP_VFPv3 | HWCAP_VFPv4 | HWCAP_NEON;
#else

#if defined(__linux__) || defined(ANDROID) || defined(MOZ_B2G)
    bool readAuxv = false;
    int fd = open("/proc/self/auxv", O_RDONLY);
    if (fd > 0) {
        struct { uint32_t a_type; uint32_t a_val; } aux;
        while (read(fd, &aux, sizeof(aux))) {
            if (aux.a_type == AT_HWCAP) {
                flags = aux.a_val;
                readAuxv = true;
                break;
            }
        }
        close(fd);
    }

    if (!readAuxv) {
        // Read the Features if the auxv is not available.
        FILE *fp = fopen("/proc/cpuinfo", "r");
        if (fp) {
            char buf[1024];
            memset(buf, 0, sizeof(buf));
            size_t len = fread(buf, sizeof(char), sizeof(buf) - 1, fp);
            fclose(fp);
            buf[len] = '\0';
            char *featureList = strstr(buf, "Features");
            if (featureList) {
                if (char *featuresEnd = strstr(featureList, "\n"))
                    *featuresEnd = '\0';
                flags = ParseARMCpuFeatures(featureList + 8);
            }
            if (strstr(buf, "ARMv7"))
                flags |= HWCAP_ARMv7;
        }
    }
#endif

    // If compiled to use specialized features then these features can be
    // assumed to be present otherwise the compiler would fail to run.

#ifdef JS_CODEGEN_ARM_HARDFP
    // Compiled to use the hardfp ABI.
    flags |= HWCAP_USE_HARDFP_ABI;
#endif

#if defined(__VFP_FP__) && !defined(__SOFTFP__)
    // Compiled to use VFP instructions so assume VFP support.
    flags |= HWCAP_VFP;
#endif

#if defined(__ARM_ARCH_7__) || defined (__ARM_ARCH_7A__)
    // Compiled to use ARMv7 instructions so assume the ARMv7 arch.
    flags |= HWCAP_ARMv7;
#endif

#endif // JS_ARM_SIMULATOR

    // Canonicalize the flags. These rules are also applied to the features
    // supplied for simulation.

    // The VFPv3 feature is expected when the VFPv3D16 is reported, but add it
    // just in case of a kernel difference in feature reporting.
    if (flags & HWCAP_VFPv3D16)
        flags |= HWCAP_VFPv3;

    // If VFPv3 or Neon is supported then this must be an ARMv7.
    if (flags & (HWCAP_VFPv3 | HWCAP_NEON))
        flags |= HWCAP_ARMv7;

    // Some old kernels report VFP and not VFPv3, but if ARMv7 then it must be
    // VFPv3.
    if (flags & HWCAP_VFP && flags & HWCAP_ARMv7)
        flags |= HWCAP_VFPv3;

    // Older kernels do not implement the HWCAP_VFPD32 flag.
    if ((flags & HWCAP_VFPv3) && !(flags & HWCAP_VFPv3D16))
        flags |= HWCAP_VFPD32;

    IonSpew(IonSpew_Codegen, "ARM HWCAP: 0x%x\n", flags);
    isSet = true;
    return flags;
}

bool HasMOVWT()
{
    return GetARMFlags() & HWCAP_ARMv7;
}
bool HasVFPv3()
{
    return GetARMFlags() & HWCAP_VFPv3;
}
bool HasVFP()
{
    return GetARMFlags() & HWCAP_VFP;
}

bool Has32DP()
{
    return GetARMFlags() & HWCAP_VFPD32;
}

bool HasIDIV()
{
    return GetARMFlags() & HWCAP_IDIVA;
}

// This is defined in the header and inlined when not using the simulator.
#if defined(JS_ARM_SIMULATOR)
bool UseHardFpABI()
{
    return GetARMFlags() & HWCAP_USE_HARDFP_ABI;
}
#endif

Registers::Code
Registers::FromName(const char *name)
{
    // Check for some register aliases first.
    if (strcmp(name, "ip") == 0)
        return ip;
    if (strcmp(name, "r13") == 0)
        return r13;
    if (strcmp(name, "lr") == 0)
        return lr;
    if (strcmp(name, "r15") == 0)
        return r15;

    for (size_t i = 0; i < Total; i++) {
        if (strcmp(GetName(i), name) == 0)
            return Code(i);
    }

    return Invalid;
}

FloatRegisters::Code
FloatRegisters::FromName(const char *name)
{
    for (size_t i = 0; i < Total; i++) {
        if (strcmp(GetName(i), name) == 0)
            return Code(i);
    }

    return Invalid;
}

FloatRegisterSet
VFPRegister::ReduceSetForPush(const FloatRegisterSet &s)
{
    FloatRegisterSet mod;
    for (TypedRegisterIterator<FloatRegister> iter(s); iter.more(); iter++) {
        if ((*iter).isSingle()) {
            // Add in just this float.
            mod.addUnchecked(*iter);
        } else if ((*iter).id() < 16) {
            // A double with an overlay, add in both floats.
            mod.addUnchecked((*iter).singleOverlay(0));
            mod.addUnchecked((*iter).singleOverlay(1));
        } else {
            // Add in the lone double in the range 16-31.
            mod.addUnchecked(*iter);
        }
    }
    return mod;
}

uint32_t
VFPRegister::GetSizeInBytes(const FloatRegisterSet &s)
{
    uint64_t bits = s.bits();
    uint32_t ret = mozilla::CountPopulation32(bits&0xffffffff) * sizeof(float);
    ret +=  mozilla::CountPopulation32(bits >> 32) * sizeof(double);
    return ret;
}
uint32_t
VFPRegister::GetPushSizeInBytes(const FloatRegisterSet &s)
{
    FloatRegisterSet ss = s.reduceSetForPush();
    uint64_t bits = ss.bits();
    uint32_t ret = mozilla::CountPopulation32(bits&0xffffffff) * sizeof(float);
    ret +=  mozilla::CountPopulation32(bits >> 32) * sizeof(double);
    return ret;
}
uint32_t
VFPRegister::getRegisterDumpOffsetInBytes()
{
    if (isSingle())
        return id() * sizeof(float);
    if (isDouble())
        return id() * sizeof(double);
    MOZ_ASSUME_UNREACHABLE();
}

uint32_t
FloatRegisters::ActualTotalPhys()
{
    if (Has32DP())
        return 32;
    return 16;
}


} // namespace jit
} // namespace js

