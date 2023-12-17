
#pragma once

// Created on 2020-08-10 by Sisco

namespace lyra
{

    enum class TextureType
    {
        Undefined                   = 0,
        Texture                     = 1 << 0,
        TextureCube                 = 1 << 1,
        ColorAttachment             = 1 << 2,
        ColorAttachmentInternal     = 1 << 3,
        DepthAttachmentInternal     = 1 << 4,
        All               = Texture | TextureCube | ColorAttachment | ColorAttachmentInternal | DepthAttachmentInternal
    };
    

}

