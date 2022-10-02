import Raylib

func main() {
    var exit = false

    Raylib.initWindow(1000, 800, "Level Editor")
    Raylib.setTargetFPS(60)

    Raylib.setExitKey(.function10)

    while !exit {
        Raylib.beginDrawing()
            Raylib.clearBackground(.gray)
            Raylib.drawText("Hello", 10, 10, 1, .black)
        Raylib.endDrawing()

        if Raylib.isKeyPressed(.escape) {
            exit = true
        }
    }

    Raylib.closeWindow()
}


main()

