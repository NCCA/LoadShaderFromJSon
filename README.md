#LoadShaderFromJSon
![alt tag](http://nccastaff.bournemouth.ac.uk/jmacey/GraphicsLib/Demos/Json.png)

This demo demonstrates how to load shaders from a json file using the new shader json loadaders in ShaderLib.

The file format for the json file is as follows

```json
{
  "ShaderProgram": {
    "name": "Phong",

  "Shaders" : [
    {
      "type": "Vertex",
      "name": "PhongVertex",
      "path" : ["shaders/version.glsl",
                "shaders/common.glsl",
                "shaders/PhongVertex.glsl"]

    },
	{
      "type": "Fragment",
      "name": "PhongFragment",
      "path" : ["shaders/version.glsl",
                "shaders/common.glsl",
                "shaders/noise3D.glsl",
                "shaders/PhongFragment.glsl"]
    }


  ]
 }
}
```
Each shader path is a list of shader sources that will be concatenated together to form a complete shader string to load. It is important that the #version is the first element loaded (in this case a file with the version string in).

Other functions etc must be loaded in correct sequence as per normal glsl shaders.

This example uses the excellent webgl noise demos from [here](https://github.com/ashima/webgl-noise) 

