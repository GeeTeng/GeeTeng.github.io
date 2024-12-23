---
title: "Piccolo LUT"
date: 2024-12-23
tags: [Game]
description: "使用LUT图实现有个性的ColorGrading效果"
showDate: true
math: true
chordsheet: true
---

#### LUT（LookUp Table）查找表

LUT是一种常见的优化或效果实现工具，用于通过查表得方式快速映射输入数据到目标结果，避免复杂的实时计算

**颜色分级（Color Grading LUT）：**

用于后期处理阶段，将场景颜色转换为特定的色彩基调。

**参考资料** [LUT.html](/file/LUT.html) 

```glsl
void main()
{
    highp ivec2 lut_tex_size = textureSize(color_grading_lut_texture_sampler, 0);
    // 纹理的高度16 和 纹理的宽度256 
    highp float _COLORS      = float(lut_tex_size.y);
    highp float xsize = float(lut_tex_size.x);

    highp vec4 color       = subpassLoad(in_color).rgba;

    // 将颜色分量映射到LUT索引范围[0, 15]
    highp float blueoffset = (_COLORS - 1.0) * color.b;
    highp float redoffset = (_COLORS - 1.0) * color.r;
    highp float greenoffset = (_COLORS - 1.0) * color.g;

    // 红色和绿色决定小块中的水平垂直位置 蓝色决定哪个小块
    highp float u = (redoffset + floor(blueoffset) * _COLORS) / xsize;
    highp float v = greenoffset / _COLORS;

    highp vec2 uv = vec2(u, v);
    highp vec4 color_sampled = texture(color_grading_lut_texture_sampler, uv);

    out_color = color_sampled;
}
```

由于条纹问题，看到别人说是因为mipmap精度问题，所以

在render_resource.cpp中修改mipmap的层级

```c++
    // create color grading texture
    rhi->createGlobalImage(
        m_global_render_resource._color_grading_resource._color_grading_LUT_texture_image,
        m_global_render_resource._color_grading_resource._color_grading_LUT_texture_image_view,
        m_global_render_resource._color_grading_resource._color_grading_LUT_texture_image_allocation,
        color_grading_map->m_width,
        color_grading_map->m_height,
        color_grading_map->m_pixels,
        color_grading_map->m_format, 1);
```

![LUT](D:\GeeSite\static\images\Piccolo\LUT.png)