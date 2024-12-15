# **Short Description**
Engine made for the UPC Master Advanced Programming for AAA Video Games.

# **Github**
https://github.com/PabioGF/Engine

# **Controls**
## **Camera**
Right-clicking and with "WASD" you can move with the camera around, if you click shift you will move faster.

You can zoom with the mouse wheel and with ALT+Left click you can orbit the object.

Pressing f you are able to focus the camera on the model loaded.

## **Model Loading**
You can drag and drop models to the engine, as well as the textures

## **UI**
There is several window to use of UI. 

At first you have the About window, in the menu bar, where you can see some information about the engine, as well as close the other window or the program itself.

Other window would be the configuration one, where you can change several aspects of some modules to change things in the scene.

Other window would be the information one, where you can find the FPS graph, as well as other information as memory consumprtion, hardware, and software.

Another information window is the model information one, where you can see some information about the model and the texture.

At last you have the console window, where you can see information about the engine process.

# **Aditional elements**
For the UI configurations, I added one for scaling the model.

Apart from zooming with the mouse wheel, you can also scale with Alt + Right click.

You can also pan by maintaining the left click held down.

# **Comments**
All the model and program generation is done from ModuleRenderExercise.

For the logs in the console, I added a vector that stores the log to use it in the ModuleEditor, so every LOG that is called is output in the console.

There is a memory leak from LoadASCIIFromFile() function that I didn't know how to solve.
