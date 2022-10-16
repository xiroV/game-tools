import Raylib

func loadLevel(_ editor: Editor) {
    // TODO Not yet implemented
}

func control() {
//func control(_ editor: Editor, windows: Window, exporters: Exporters) {
    // TODO Not yet implemented
}

func drawMessages() {

}

func main() {
    var exit = false

    let window = Window(
        width: 1600,
        height: 1000,
        title: "Level Editor"
    )
    
    var camera = Camera2D()
    camera.target = Vector2(x: Float(window.width)/2, y: Float(window.height)/2);
    camera.offset = Vector2(x: Float(window.width)/2, y: Float(window.height)/2);
    camera.rotation = 0.0;
    camera.zoom = 1.0;

    let editor = Editor(
        version: 1,
        outputDelimiter: ";",
        window: window,
        fontSize: 20,
        camera: camera
    )
    
    let qc = QuakeConsole()


    Raylib.initWindow(editor.window.width, editor.window.height, editor.window.title)
    Raylib.setTargetFPS(60)

    Raylib.setExitKey(.null)

    // TODO(Brian) Figure font out later
    //let fontDefault = Raylib.loadFontEx("assets/fonts/OverpassMono/OverpassMono-Regular.ttf", editor.fontSize, 0, 0);
    //SetTextureFilter(fontDefault.texture, TEXTURE_FILTER_BILINEAR);
    //GuiSetFont(fontDefault);

    if (CommandLine.arguments.count > 1) {
        editor.levelName = CommandLine.arguments.first!
        loadLevel(editor)
    } else {
        editor.levelName = "TestLevel1.lvl"
        loadLevel(editor)
    }



    while !exit {
        editor.control(window: window)

        /*
        camera.zoom += cameraZoom;
        if (camera.zoom > 2.0f) {
            cameraZoom = 2.0f;
            camera.zoom = cameraZoom;
        }
        else if (camera.zoom < 0.5f) {
            cameraZoom = 0.5f;
            camera.zoom = cameraZoom;
        }

        camera.target = editor.cameraTarget;
        */

        Raylib.beginDrawing()
            Raylib.clearBackground(.darkGray)
            editor.drawGrid()
            Raylib.beginMode2D(editor.camera)
                editor.drawObjects()
            Raylib.endMode2D()
            //drawHelp(&editor);
            //drawWindows(&editor, &windows, exporters)
            qc.render(window: window)
        
            if (editor.showFPS) { Raylib.drawFPS(20, 20) }
            //drawMessages(&editor);
        Raylib.endDrawing()

        editor.camera.zoom = 1
        //updateEditor(&editor);
        
        qc.input()
        if !qc.isOpen {
            if Raylib.isKeyPressed(.escape) {
                exit = true
            }
        }
        
        qc.update()
    }

    Raylib.closeWindow()
}


main()

