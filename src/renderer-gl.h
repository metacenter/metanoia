// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_RENDERER_GL_H
#define NOIA_RENDERER_GL_H

#include "renderer.h"
#include "utils-gl.h"

/// Implementation of rendering strategy for GL backend.
/// @see NoiaRenderer, NoiaRendererMMap
typedef struct NoiaRendererGLInternal NoiaRendererGL;

/// NoiaRendererGL constructor.
NoiaRenderer* noia_renderer_gl_create(NoiaEGLBundle* egl, NoiaSize size);

#endif // NOIA_RENDERER_GL_H

