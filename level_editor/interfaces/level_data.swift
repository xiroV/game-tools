class ObjectData {
    let key: String 
    let value: String

    init(key: String, value: String) {
        self.key = key
        self.value = value
    }
}

class LevelObject {
    let x, y, width, height, rotation: Int32
    let type: String
    let data: Array<ObjectData>

    init(
        _ x: Int32,
        _ y: Int32,
        _ width: Int32,
        _ height: Int32,
        _ rotation: Int32,
        _ type: String,
        _ data: Array<ObjectData> = []
    ) {
        self.x = x
        self.y = y 
        self.width = width
        self.height = height
        self.rotation = rotation
        self.type = type 
        self.data = data 
        
    }
}

protocol LevelData {
    func getObjects() -> Array<LevelObject>
}
