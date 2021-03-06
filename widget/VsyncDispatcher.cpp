/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "VsyncDispatcher.h"
#include "mozilla/ClearOnShutdown.h"
#include "mozilla/layers/CompositorParent.h"
#include "gfxPrefs.h"

#ifdef MOZ_ENABLE_PROFILER_SPS
#include "GeckoProfiler.h"
#include "ProfilerMarkers.h"
#endif

#ifdef MOZ_WIDGET_GONK
#include "GeckoTouchDispatcher.h"
#endif

using namespace mozilla::layers;

namespace mozilla {

StaticRefPtr<VsyncDispatcher> sVsyncDispatcher;

/*static*/ VsyncDispatcher*
VsyncDispatcher::GetInstance()
{
  if (!sVsyncDispatcher) {
    sVsyncDispatcher = new VsyncDispatcher();
    ClearOnShutdown(&sVsyncDispatcher);
  }

  return sVsyncDispatcher;
}

VsyncDispatcher::VsyncDispatcher()
  : mCompositorObserverLock("CompositorObserverLock")
{

}

VsyncDispatcher::~VsyncDispatcher()
{
  MutexAutoLock lock(mCompositorObserverLock);
  mCompositorObservers.Clear();
}

void
VsyncDispatcher::SetVsyncSource(VsyncSource* aVsyncSource)
{
  mVsyncSource = aVsyncSource;
}

void
VsyncDispatcher::DispatchTouchEvents(bool aNotifiedCompositors, TimeStamp aVsyncTime)
{
  // Touch events can sometimes start a composite, so make sure we dispatch touches
  // even if we don't composite
#ifdef MOZ_WIDGET_GONK
  if (!aNotifiedCompositors && gfxPrefs::TouchResampling()) {
    GeckoTouchDispatcher::NotifyVsync(aVsyncTime);
  }
#endif
}

void
VsyncDispatcher::NotifyVsync(TimeStamp aVsyncTimestamp)
{
  bool notifiedCompositors = false;
#ifdef MOZ_ENABLE_PROFILER_SPS
  if (profiler_is_active()) {
    CompositorParent::PostInsertVsyncProfilerMarker(aVsyncTimestamp);
  }
#endif

  DispatchTouchEvents(notifiedCompositors, aVsyncTimestamp);
}

bool
VsyncDispatcher::NotifyVsyncObservers(TimeStamp aVsyncTimestamp, nsTArray<nsRefPtr<VsyncObserver>>& aObservers)
{
  // Callers should lock the respective lock for the aObservers before calling this function
  for (size_t i = 0; i < aObservers.Length(); i++) {
    aObservers[i]->NotifyVsync(aVsyncTimestamp);
 }
 return !aObservers.IsEmpty();
}

void
VsyncDispatcher::AddCompositorVsyncObserver(VsyncObserver* aVsyncObserver)
{
  MOZ_ASSERT(CompositorParent::IsInCompositorThread());
  MutexAutoLock lock(mCompositorObserverLock);
  if (!mCompositorObservers.Contains(aVsyncObserver)) {
    mCompositorObservers.AppendElement(aVsyncObserver);
  }
}

void
VsyncDispatcher::RemoveCompositorVsyncObserver(VsyncObserver* aVsyncObserver)
{
  MOZ_ASSERT(CompositorParent::IsInCompositorThread() || NS_IsMainThread());
  MutexAutoLock lock(mCompositorObserverLock);
  if (mCompositorObservers.Contains(aVsyncObserver)) {
    mCompositorObservers.RemoveElement(aVsyncObserver);
  } else {
    NS_WARNING("Could not delete a compositor vsync observer\n");
  }
}

} // namespace mozilla
