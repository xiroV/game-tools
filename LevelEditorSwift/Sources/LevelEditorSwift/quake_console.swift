import Raylib

let ConsoleHeightPercent: Float32 = 0.20;

class QuakeConsole {
    var isOpen: Bool = false
    var openT: Float32 = 0.0
    var currentCommand: [Character] = []
    var history: [String]
    var currentHistoryEntry = -1
    
    init() {
        self.isOpen = false
        self.openT = 0.0
        self.currentCommand = []
        self.history = []
        self.currentHistoryEntry = -1
    }

    func update() {
        let dt: Float32 = Raylib.getFrameTime()
        if self.isOpen {
            self.openT += dt * 3
        } else {
            self.openT -= dt * 3
        }
        self.openT = clamp(self.openT, 0, 1)
    }

    func input() {
        if Raylib.isKeyPressed(.grave) {
            self.isOpen = !self.isOpen
            return
        }

        if !self.isOpen {return;}

        if Raylib.isKeyPressed(.enter) {
            if self.currentCommand.count != 0 {
                let asString = String(self.currentCommand)
                self.history.append(asString)
                self.currentCommand.removeAll()
                self.parseAndExecuteCommand()
                self.currentHistoryEntry = -1
                return
            }
        }

        if Raylib.isKeyPressed(.up) {
            self.currentHistoryEntry += 1
            self.currentHistoryEntry = clamp(self.currentHistoryEntry, -1, self.history.count - 1)
            if self.currentHistoryEntry != -1 {
                moveStringToCurrentCommand(command: self.history[self.history.count - self.currentHistoryEntry - 1])
            }
        }

        if Raylib.isKeyPressed(.down) {
            self.currentHistoryEntry -= 1
            self.currentHistoryEntry = clamp(self.currentHistoryEntry, -1, self.history.count - 1)
            if self.currentHistoryEntry != -1 {
                moveStringToCurrentCommand(command: self.history[self.history.count - self.currentHistoryEntry - 1])
            }
        }


        guard var key = Raylib.getCharPressed() else {
            return
        }
        
        guard var keyValue = key.asciiValue else {
            return
        }
        
        while keyValue > 0 {
            if keyValue >= 32 && keyValue <= 125 {
                self.currentCommand.append(key)
            }
            self.currentHistoryEntry = -1;
            
            guard let newKey = Raylib.getCharPressed() else {
                return
            }
            
            guard let newKeyValue = newKey.asciiValue else {
                return
            }
            
            key = newKey;
            
            keyValue = newKeyValue
        }

        if Raylib.isKeyPressed(.backspace) {
            _ = self.currentCommand.popLast()
        }
    }

    func render(window: Window) {
        var bottom = (ConsoleHeightPercent * Float(window.height) * self.openT)
        Raylib.drawRectangle(0, 0, Int32(window.width), Int32(bottom), .brown)
        bottom -= 28
        let isEmpty = self.currentCommand.isEmpty
        Raylib.drawText(isEmpty ? "Start typing..." : String(self.currentCommand), 4, Int32(bottom), 24, isEmpty ? .gray : .white)
        if self.history.count == 0 {return}
        for it in 1...self.history.count  {
            bottom -= 28
            let index = self.history.count - it
            Raylib.drawText(self.history[index], 4, Int32(bottom), 24, index == (self.history.count - self.currentHistoryEntry - 1) ? .blue : .gray)
        }
    }

    func parseAndExecuteCommand() {
        let tokenized = self.history[self.history.count - 1].split(separator: " ");
        if tokenized.count == 0 {return} // Shouldn't be possible, but let's handle it.
        let first = tokenized[0]
        switch first {
            case "clear_history":
                self.history.removeAll()
            default:
                ()
        }
    }

    func moveStringToCurrentCommand(command: String) {
        self.currentCommand.removeAll()
        for ch in command {
            self.currentCommand.append(ch)
        }
    }
};

func clamp(_ input: Int, _ lower: Int, _ upper: Int) -> Int {
    if input < lower { return lower }
    if input > upper { return upper }
    return input
}

func clamp(_ input: Float32, _ lower: Float32, _ upper: Float32) -> Float32 {
    if input < lower { return lower }
    if input > upper { return upper }
    return input
}
