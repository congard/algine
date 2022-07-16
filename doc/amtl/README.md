# Algine Material Library (AMTL)

- Used to **complement** the default model materials
- AMTL has the highest priority, so if the value is in AMTL, then it will be used
- AMTL is a simple lua file, in which you can override (or create new) model material values

## Example

```lua
-- demo.amtl

material("material0")
ambient("tex.jpg", "tex0")  -- load tex0.jpg as public with name 'tex0'
diffuse("tex.jpg", "tex0")  -- use public texture with name 'tex0'
specular(Name("tex0"))  -- use public texture with name 'tex0'
normal("norm.png")  -- load norm0.png as private
set("myCustomValue", 3.14)

params = {}
params[Texture.Param.MinFilter] = Texture.Value.Linear
params[Texture.Param.MagFilter] = Texture.Value.Linear
params[Texture.Param.WrapU] = Texture.Value.Repeat
params[Texture.Param.WrapV] = Texture.Value.Repeat

texParams(params)  -- load textures with default params 'params'

material("material1")
ambient("tex.jpg", "tex1")
diffuse("tex.jpg", "tex1")
specular(Name("tex1"))
normal("norm.png")
reflection("reflection.lua")  -- you can also pass paths to scripts, and even Texture2DCreator
```
