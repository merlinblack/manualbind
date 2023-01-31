Lua 5.4
=======

Lua 5.4 allows you to annotate local varibles to either make them const, or const and closing.
Closing varibles when they go out of scope will call thier __close metamethod.

The binding now handles __close in addition to __gc, by resetting the shared pointer, effectively releasing that refcount for the object. This allows you to have more deterministic releasing of resources, without having to force garbage collections.

Example
----------
Say you have a binding for a texure class which looks after allocated memory on the GPU in addition to normal ram. You want this to be cleaned up ASAP after use.

```Lua
function setCaption(dialogbox, text)
	local texture <close> = generateTextureFromText(text)
	dialogbox.setCaptionTexture(texture)
end
```

Once this function finishes - normally without the <close> annotation, the texture var becomes garbage and *eventually* it's __gc metamethod called which will destroy the shared pointer - reducing the ref count.

However with the <close> annotation, at the end of the function as 'texture' goes out of scope, it's __close metamethod will be called. This calls std::shared_ptr<T>.reset() which will reduce the ref count. This happens as soon as the function finishes. Later, the __gc metamethod will destroy an empty (pointing at nullptr) shared pointer.
