# BasicWidgets

Supported systems: Linux, Windows

This very basic example shows how to use basic widgets using XML.

> [!NOTE]
> Working directory must be set to the `resources` directory.

This example uses
[Noto Sans Black](https://fonts.google.com/noto/specimen/Noto+Sans) font.

## Introduction

Algine is a modular, easy-to-use 3D rendering engine. Like any other
framework, Algine has its own features.

### Objects

All classes are divided into two types: objects and ordinary classes.
Objects are special classes, which inherit the `algine::Object` class.
Such classes have some important features: they can have many children,
but only one parent. All objects by default have `algine::GlobalScene`
as the parent and must be created on the heap, using the `new` operator.

#### Lifecycle

As mentioned before, objects must be created using operator `new`.

> [!IMPORTANT]  
> Allocating on the heap is crucial, since it will allow to correctly
> destroy all children while destroying the parent. Otherwise, you will
> get a segmentation fault.

If you want to manually delete an object, you can use operator `delete`.
In that case, all its children will be also deleted, moreover, the object
will be automatically removed from the children list of its parent.

> [!WARNING]
> If you want to use objects like regular classes, you can just pass
> `nullptr` as the parent. However, it is highly unrecommended and can be
> really dangerous. Do it on your own risk.

### Display hierarchy

Okay, now you already know what is an object. But how we can render things?

#### `Window`

As the name suggests, `Window` is a Window. It is the base class for
platform-depended windows like e.g. `GLFWWindow` or `QtWindow`. This class
receives events, executes tasks, is responsible for the render loop. But
how exactly this class renders your content?

#### `Content`

`Content` is an entry point for rendering. It has an abstract function
`render` which needs to be overridden. It also has virtual functions like
`onShow` and `onHide`. `Content` has width and height, received from the
window. But how we can receive more events?

##### `UnifiedEventHandler`

`UnifiedEventHandler` is a cross-platform event handler. Typically, you
want your custom content to inherit this class, and then, somewhere in
`YourContent::onShow`, register a new event handler by using

```cpp
getWindow()->setEventHandler(this);
```

Interesting, but `Content` will not render my scene, will it?

#### `Scene`

As the name suggests, this is scene. Scene is an object, which has some
useful functions like `show`, `hide`, `render`, `addChild` etc. `Scene`
represents your scene. Feel free to create as many class members as you
wish. By default, a scene is hidden, so you should call `show` before
`render`, if you want to see something, of course. But can I render my
whole scene just here? Well, yes, but no. In order to render something,
you need to use `Renderer`.

#### `Renderer` and `RenderPass`

Finally! `Renderer` performs `RenderPass` rendering. `RenderPass` has
virtual function `render`, which needs to be overridden. So yes, now you
can place all your rendering code in `YourRenderPass::render` and add it
to your renderer. Do not forget to call `Renderer::render` in `YourSceene`!

## Summary

So, generally speaking, normally we want to have the following structure:

- `Window`
  - `Content`
  - `UnifiedEventHandler`
    - `Scene`
      - `Renderer`
        - `RenderPass`

Of course, in the current engine architecture, some stages are not obligatory,
but who knows, what will be changed in the future?

## Conclusion

After reading this loooong document, you may have some basic knowledge about
some parts of the engine. Congratulations! Now, you can enjoy the code :)

[//]: # (TODO: move this documentation to the HelloCube example)
