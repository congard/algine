# Lua Support Roadmap

## Why do we need Lua?

Lua is a simple, small, fast, easy to learn embeddable language. It can be very useful in
projects such as Algine, since there are a lot of things that we can do much easier with language like Lua.

Firstly, we need Lua to get rid of awful, overcomplicated, inflexible, hard to maintain
json configs that we are now using in Creators.

Secondly, we need Lua to simplify developer's lives - not all projects need to be written in
C++, so in this case easy to learn language like Lua would be a great solution.

And finally, we need Lua to create Editor, since it will much easier to create it using language like Lua.

## Classes

**An approximate** list of classes that need lua support to be implemented:

Creators:

- [x] Creator
- [x] ShaderCreator
- [x] ShaderDefinitionGenerator
- [x] ShaderProgramCreator
- [ ] TextureCreator
- [ ] Texture2DCreator
- [ ] TextureCubeCreator
- [ ] ImageCreator
- [ ] FramebufferCreator
- [ ] RenderbufferCreator
- [ ] OutputListCreator
- [ ] InputLayoutShapeLocationsCreator
- [ ] ModelCreator
- [ ] ShapeCreator

Core:

- [ ] Engine
- [x] Object
- [x] Log
- [x] IOProvider
- [ ] IOSystem
- [ ] IOStream
- [ ] StandardIOStream
- [ ] StandardIOSystem
- [ ] AssetsIOStream
- [ ] AssetsIOSystem
- [ ] Buffer
- [ ] ArrayBuffer
- [ ] IndexBuffer
- [ ] UniformBuffer
- [x] Shader
- [x] ShaderProgram
- [ ] Texture
- [ ] Texture2D
- [ ] TextureCube
- [ ] Framebuffer
- [ ] Renderbuffer
- [ ] InputLayout
- [ ] OutputList
- [ ] Resources

Widgets:

- [ ] Paint
- [ ] Painter
- [ ] RoundRect
- [ ] Alignment
- [ ] Container
- [ ] Dimen
- [ ] ImageWidget
- [ ] Label
- [ ] Layout
- [ ] LinearLayout
- [ ] RelativeLayout
- [ ] SizePolicy
- [ ] Unit
- [ ] Units
- [ ] Widget

Std:

- [ ] InputLayoutShapeLocations
- [ ] Mesh
- [ ] Model
- [ ] Shape

Glm:

- [x] vec3
- [x] vec4
- [x] mat3
- [x] mat4
- [x] quat
- [x] basic functions

Tulz:

- [x] UnsignedArray
- [x] IntArray
- [x] FloatArray
- [x] DoubleArray

In reality this list will be much bigger

## TODOs

- [x] Redefine Lua's `require` to load Algine types (base, widgets, core (base+widgets), std, glm, tulz), e.g. `require('algine:core')`

## Useful pages

* Sol documentation: https://sol2.readthedocs.io/en/latest/
* Sol - C++ in Lua - basic class binding example: https://sol2.readthedocs.io/en/latest/tutorial/cxx-in-lua.html
* Sol - property: https://sol2.readthedocs.io/en/latest/api/property.html
* Sol - usertype: https://sol2.readthedocs.io/en/latest/api/usertype.html
* Sol - ownership: https://sol2.readthedocs.io/en/latest/tutorial/ownership.html
* Programming in Lua: https://www.lua.org/pil/contents.html
* Lua OOP: https://www.tutorialspoint.com/lua/lua_object_oriented.htm
* How to pass table as vector to a c++ function: https://github.com/ThePhD/sol2/issues/1054
* Virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance

## Hints

* To create binding to overloaded function use `static_cast<return_type (T::*)(args)>(&T::function_name)`
* To create binding to overloaded static function use `static_cast<return_type (*)(args)>(&T::function_name)`
* If you bind class member function to lambda, then the 1st argument will be self:
  `usertype["function"] = [](const sol::object &self, ...) { self.as<T>().function(...); };`
* `Lua::new_enum` allows you to create an enum and then make it as a class member
* `Lua::new_property` creates property and additionally makes getter and setter
* If you inherit `Scriptable`, then you also need to define `static void registerLuaUsertype(Lua *lua)`