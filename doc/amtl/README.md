# Algine Material Library (AMTL)
Used to complement conventional (classic) model materials.
* AMTL, in addition to the usual ones, uses some special parameters
* AMTL has the highest priority, so if the value is in AMTL, then it is used,
and the value of the classical material is ignored
* To use the value from AMTL, the material name must coincide with what is in
the classical material of the model (or the model itself), which you want to change/supplement

## AMTL structure
AMTL is a JSON array that contains objects - materials. You can see example [here](example.amtl.json)

* **name**: String
* **ambientStrength**, **diffuseStrength**, **specularStrength**,
**shininess**, **reflection** and **jitter**: Float (optional)
* **ambientTexture**, **diffuseTexture**, **specularTexture**,
**normalTexture**, **reflectionTexture** and **jitterTexture**: Object (optional)
  * **path**: String
  * **sharedLevel**: String, one of `unique`, `materialShared`, `modelShared` or `shared` (optional)
  * **shareWith**: Array of Strings, must be present if sharedLevel set to materialShared
  * **params**: Object (optional, all params are optional too)
    * **wrapU**: String, one of `repeat`, `clampToEdge`, `clampToBorder`, `mirroredRepeat` or `mirrorClampToEdge`
    * **wrapV**: String, one of `repeat`, `clampToEdge`, `clampToBorder`, `mirroredRepeat` or `mirrorClampToEdge`
    * **minFilter**: String, one of `nearest` or `linear`
    * **magFilter**: String, one of `nearest` or `linear`