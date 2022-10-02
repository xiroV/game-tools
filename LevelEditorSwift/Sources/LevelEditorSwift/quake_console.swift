import Raylib

let ConsoleHeightPercent: Float32 = 0.20;

class QuakeConsole {
    var isOpen: Bool = false;
    var openT: Float32 = 0.0;
    var currentCommand: [UInt8];
    var history: [String];
    var currentHistoryEntry = -1;
    
    init() {
        self.currentCommand[0] = 0;
    }

    func update() {
        let dt: Float32 = Raylib.getFrameTime();
        if self.isOpen {
            self.openT += dt * 3;
        } else {
            self.openT -= dt * 3;
        }
        if self.openT < 0.0 {
            self.openT = 0.0;
        } else if self.openT > 1.0 {
            self.openT = 1;
        }
    }

    func input() {
        if Raylib.isKeyPressed(.grave) {
            self.isOpen = !self.isOpen;
            return;
        }

        if !self.isOpen {return;}

        if Raylib.isKeyPressed(.enter) {
            if self.currentCommand[0] != 0 {
                var asString = String(decoding: self.currentCommand, as: UTF8.self)
                self.history.append(asString);
                self.currentCommand[0] = 0;
                self.parseAndExecuteCommand();
                self.currentHistoryEntry = -1;
                return;
            }
        }

        if Raylib.isKeyPressed(.up) {
            self.currentHistoryEntry += 1;
            self.currentHistoryEntry = clamp(self.currentHistoryEntry, -1, self.history.count - 1);
            if self.currentHistoryEntry != -1 {
                moveStringToCurrentCommand(command: self.history[self.history.count - self.currentHistoryEntry - 1]);
            }
        }

        if Raylib.isKeyPressed(.down) {
            self.currentHistoryEntry -= 1;
            self.currentHistoryEntry = clamp(self.currentHistoryEntry, -1, self.history.count - 1);
            if self.currentHistoryEntry != -1 {
                moveStringToCurrentCommand(command: self.history[self.history.count - self.currentHistoryEntry - 1]);
            }
        }


        guard var key = Raylib.getCharPressed()?.wholeNumberValue else {
            return;
        }
        
        while key > 0 {
            if (key >= 32) && (key <= 125) {
                var counter = 0;
                while self.currentCommand[counter] != 0 && counter < self.currentCommand.count - 1 {counter += 1;}
                if counter != self.currentCommand.count - 1 {
                    self.currentCommand[counter] = UInt8(key);
                    self.currentCommand[counter + 1] = 0;
                }
            }
            self.currentHistoryEntry = -1;
            
            guard var newKey = Raylib.getCharPressed()?.wholeNumberValue else {
                return;
            }
            
            key = newKey;
        }

        if Raylib.isKeyPressed(.backspace) {
            self.currentHistoryEntry = -1;
            var counter = 0;
            while self.currentCommand[counter] != 0 && counter < self.currentCommand.count + 1 {counter += 1;}
            if counter != 0 {
                self.currentCommand[counter - 1] = 0;
            }
        }

    }

    func render(screenWidth: Int, screenHeight: Int) {
        var bottom = (ConsoleHeightPercent * Float(screenHeight) * self.openT);
        Raylib.drawRectangle(0, 0, Int32(screenWidth), Int32(bottom), .brown);
        bottom -= 28;
        var isEmpty = self.currentCommand[0] == 0;
        Raylib.drawText(isEmpty ? "Start typing..." : String(decoding: self.currentCommand, as: UTF8.self), 4, Int32(bottom), 24, isEmpty ? .gray : .white);
        for it in 1...self.history.count  {
            bottom -= 28;
            let index = self.history.count - it;
            Raylib.drawText(self.history[index], 4, Int32(bottom), 24, index == (self.history.count - self.currentHistoryEntry - 1) ? .blue : .gray);
        }
    }

    func parseAndExecuteCommand() {
        var tokenized = self.history[self.history.count - 1].split(separator: " ");
        if tokenized.count == 0 {return;} // Shouldn't be possible.
        var first = tokenized[0];
        switch first {
            case "clear_history":
                self.history = [];
            default:
                ();
        }
    }

    func moveStringToCurrentCommand(command: String) {
        for it in 0...command.count - 1 {
            // self.currentCommand[it] = command[it]?.wholeNumberValue;
            _ = it;
            (/* TODO */);
            
        }
        
        self.currentCommand[command.count] = 0;
    }
};

func clamp(_ input: Int, _ lower: Int, _ upper: Int) -> Int {
    if input < lower { return lower; }
    if input > upper { return upper; }
    return input;
}
