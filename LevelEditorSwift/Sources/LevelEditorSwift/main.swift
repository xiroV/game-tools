import Raylib

var screenWidth = 1000
var screenHeight = 800

func main() {
    var exit = false

    Raylib.initWindow(800, 1000, "Level Editor")
    Raylib.setTargetFPS(60)
    
    var qc = QuakeConsole()

    Raylib.setExitKey(.function10)

    while !exit {
        Raylib.beginDrawing()
            Raylib.clearBackground(.gray)
            Raylib.drawText("Hello", 10, 10, 1, .black)
            qc.render(screenWidth: screenWidth, screenHeight: screenHeight)
        Raylib.endDrawing()

        qc.update()
        if !qc.isOpen {
            if Raylib.isKeyPressed(.escape) {
                
                exit = true
            }
        }
    }

    Raylib.closeWindow()
}


main()

