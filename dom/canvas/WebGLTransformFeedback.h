/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef WEBGL_TRANSFORM_FEEDBACK_H_
#define WEBGL_TRANSFORM_FEEDBACK_H_

#include "mozilla/LinkedList.h"
#include "nsWrapperCache.h"
#include "WebGLBindableName.h"
#include "WebGLObjectModel.h"

namespace mozilla {

class WebGLTransformFeedback MOZ_FINAL
    : public nsWrapperCache
    , public WebGLBindableName<GLenum>
    , public WebGLRefCountedObject<WebGLTransformFeedback>
    , public LinkedListElement<WebGLTransformFeedback>
    , public WebGLContextBoundObject
{
    friend class WebGLContext;

public:
    explicit WebGLTransformFeedback(WebGLContext* webgl);

    void Delete();
    WebGLContext* GetParentObject() const;
    virtual JSObject* WrapObject(JSContext* cx) MOZ_OVERRIDE;

    NS_INLINE_DECL_CYCLE_COLLECTING_NATIVE_REFCOUNTING(WebGLTransformFeedback)
    NS_DECL_CYCLE_COLLECTION_SCRIPT_HOLDER_NATIVE_CLASS(WebGLTransformFeedback)

private:
    ~WebGLTransformFeedback();
};

} // namespace mozilla

#endif // WEBGL_TRANSFORM_FEEDBACK_H_
