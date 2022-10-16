import Raylib

func drawRect(_ x: Float, _ y: Float, _ width: Float, _ height: Float, _ rotation: Float, _ color: Color) {
    Raylib.drawRectanglePro(
        Rectangle(x: x + width / 2.0, y: y + height / 2.0, width: width, height: height),
        Vector2(x: width / 2.0, y: height / 2.0),
        rotation,
        color
    );
}
