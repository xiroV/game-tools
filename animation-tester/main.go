/*
Animation Test Utility

Utility for testing animations. The following features should be supported:

  - [x] Adding picture files by drag and drop
  - [ ] Rearranging the frames
  - [ ] Player controls (start pause)
  - [ ] Adjusting the playback speed
  - [ ] Adjusting the zoom
  - [ ] Adjusting the background color
*/
package main

import (
	_ "embed"
	"fmt"
	"math"

	rl "github.com/gen2brain/raylib-go/raylib"
)

//go:embed assets/fonts/Noto_Sans/static/NotoSans-Light.ttf
var NotoSansFontData []byte

type Mode string

var (
	ModeEdit Mode = "EDIT"
	ModePlay Mode = "PLAY"
)

type Fonts struct {
	defaultFont rl.Font
}

type Data struct {
	textures    []rl.Texture2D
	frame       int
	interval    float32
	lastUpdate  float32
	background  rl.Color
	colorPicker []ColorPickerOption
	mode        Mode
	fonts       Fonts
}

type ColorPickerOption struct {
	rec   rl.Rectangle
	color rl.Color
}

const (
	screenWidth  int = 1280
	screenHeight int = 720
)

var (
	colorPicker []ColorPickerOption
)

func drawText(data *Data, text string, x int, y int) {
	rl.DrawTextEx(data.fonts.defaultFont, text, rl.NewVector2(float32(x), float32(y)), 20, 0, rl.Black)
}

var (
	minusSpeedButton rl.Rectangle = rl.NewRectangle(0, float32(screenHeight)-30, 30, 30)
	plusSpeedButton  rl.Rectangle = rl.NewRectangle(85, float32(screenHeight)-30, 30, 30)
	speedText        rl.Rectangle = rl.NewRectangle(35, float32(screenHeight)-30, 80, 30)
	canvas           rl.Rectangle = rl.NewRectangle(0, 120, float32(screenWidth)-91, float32(screenHeight)-150)
	playPauseButton  rl.Rectangle = rl.NewRectangle(114, float32(screenHeight)-30, 60, 30)
)

func computeColorPicker() []ColorPickerOption {
	xPos := screenWidth - 90
	yPos := 120
	perColumn := 3
	size := 30

	var result []ColorPickerOption

	var xOffset int32 = int32(xPos)
	var yOffset int32 = int32(yPos)
	colors := [21]rl.Color{
		rl.White, rl.LightGray, rl.Gray,
		rl.Yellow, rl.Gold, rl.Orange,
		rl.Green, rl.Lime, rl.DarkGreen,
		rl.SkyBlue, rl.Blue, rl.DarkBlue,
		rl.Pink, rl.Red, rl.Maroon,
		rl.Beige, rl.Brown, rl.DarkBrown,
		rl.Purple, rl.Violet, rl.DarkPurple,
	}

	i := 0
	for _, color := range colors {
		rec := rl.NewRectangle(float32(xOffset), float32(yOffset), float32(size), float32(size))

		result = append(result, ColorPickerOption{rec: rec, color: color})

		if i+1 > perColumn-1 {
			yOffset += int32(size)
			xOffset = int32(xPos)
			i = 0
		} else {
			xOffset += int32(size)
			i++
		}
	}

	return result
}

func draw(data *Data) {
	rl.DrawLine(0, 120, int32(screenWidth), 120, rl.Black)
	rl.DrawLine(0, int32(screenHeight)-29, int32(screenWidth), int32(screenHeight)-29, rl.Black)
	rl.DrawLine(canvas.ToInt32().Width+1, canvas.ToInt32().Y, canvas.ToInt32().Width+1, int32(screenHeight)-30, rl.Black)

	// Timeline view
	offsetX := 10

	for index, texture := range data.textures {
		displayWidth := texture.Width
		displayHeight := texture.Height

		if displayWidth > 150 {
			displayWidth = 150
			displayHeight = texture.Height / int32(math.Ceil(float64(texture.Width)/150))
		}

		if displayHeight > 100 {
			displayHeight = 100
			displayWidth = texture.Width / int32(math.Ceil(float64(texture.Height)/100))
		}

		rl.DrawTexturePro(
			texture,
			rl.NewRectangle(0, 0, float32(texture.Width), float32(texture.Height)),
			rl.NewRectangle(float32(offsetX), 10, float32(displayWidth), float32(displayHeight)),
			rl.NewVector2(0, 0),
			0,
			rl.White,
		)

		if index == data.frame {
			rl.DrawRectangleLines(int32(offsetX), 10, displayWidth, displayHeight, rl.LightGray)
		}

		offsetX += int(displayWidth)
	}

	// Canvas
	rl.BeginScissorMode(canvas.ToInt32().X, canvas.ToInt32().Y, canvas.ToInt32().Width, canvas.ToInt32().Height)
	rl.DrawRectangleRec(canvas, data.background)
	if len(data.textures) > 0 {
		texture := data.textures[data.frame]

		// find scale
		displayWidth := texture.Width
		displayHeight := texture.Height

		if displayWidth > canvas.ToInt32().Width {
			displayWidth = int32(canvas.Width)
			displayHeight = texture.Height / int32(math.Ceil(float64(displayWidth)/float64(canvas.Width)))
		}

		if displayHeight > canvas.ToInt32().Height {
			displayHeight = canvas.ToInt32().Height
			displayWidth = displayWidth / int32(math.Ceil(float64(displayHeight)/float64(canvas.Height)))
		}

		rl.DrawTexturePro(
			texture,
			rl.NewRectangle(0, 0, float32(texture.Width), float32(texture.Height)),
			rl.NewRectangle(0, 120, float32(displayWidth), float32(displayHeight)),
			rl.NewVector2(0, 0),
			0,
			rl.White,
		)
	}
	rl.EndScissorMode()

	// Buttons
	for _, color := range data.colorPicker {
		rl.DrawRectangleRec(color.rec, color.color)
	}

	rl.DrawRectangleRec(minusSpeedButton, rl.Gray)
	rl.DrawRectangleLinesEx(minusSpeedButton, 1, rl.Black)
	rl.DrawText("-", minusSpeedButton.ToInt32().X+10, minusSpeedButton.ToInt32().Y+5, 20, rl.Black)

	rl.DrawRectangleRec(plusSpeedButton, rl.Gray)
	rl.DrawRectangleLinesEx(plusSpeedButton, 1, rl.Black)
	rl.DrawText("+", plusSpeedButton.ToInt32().X+10, plusSpeedButton.ToInt32().Y+5, 20, rl.Black)

	drawText(data, fmt.Sprintf("%.0fms", data.interval*1000), int(speedText.X), int(speedText.Y)+5)

	rl.DrawRectangleRec(playPauseButton, rl.Gray)
	rl.DrawRectangleLinesEx(playPauseButton, 1, rl.Black)

	if data.mode == ModeEdit {
		drawText(data, "Play", int(playPauseButton.X)+10, int(playPauseButton.Y)+5)
	} else {
		drawText(data, "Pause", int(playPauseButton.X)+10, int(playPauseButton.Y)+5)
	}

}

func control(data *Data) {
	delta := rl.GetFrameTime()

	if rl.IsFileDropped() {
		droppedFiles := rl.LoadDroppedFiles()

		if len(droppedFiles) > 0 {
			for _, f := range droppedFiles {
				image := rl.LoadImage(f)
				texture := rl.LoadTextureFromImage(image)
				data.textures = append(data.textures, texture)
			}
		}
	}

	if data.mode == ModePlay {
		if data.lastUpdate >= data.interval {
			data.frame += 1
			data.lastUpdate = 0
		}

		if data.frame >= len(data.textures) {
			data.frame = 0
		}

		data.lastUpdate += delta
	}

	if data.mode == ModeEdit {
		if len(data.textures) > 0 {
			if rl.IsKeyPressed(rl.KeyLeft) {
				data.frame -= 1

				if data.frame < 0 {
					data.frame = len(data.textures) - 1
				}
			}

			if rl.IsKeyPressed(rl.KeyRight) {
				data.frame += 1

				if data.frame >= len(data.textures) {
					data.frame = 0
				}
			}
		}
	}

	if rl.IsKeyPressed(rl.KeyUp) {
		data.interval += 0.01
	}

	if rl.IsKeyPressed(rl.KeyDown) {
		data.interval -= 0.01
	}

	if (rl.IsKeyPressed(rl.KeyDelete) || rl.IsKeyPressed(rl.KeyBackspace)) && len(data.textures) > 0 {
		newTextures := []rl.Texture2D{}
		for index, texture := range data.textures {
			if data.frame == index {
				continue
			}
			newTextures = append(newTextures, texture)
		}

		data.textures = newTextures
	}

	if rl.IsKeyPressed(rl.KeySpace) {
		if data.mode == ModeEdit {
			data.mode = ModePlay
		} else {
			data.mode = ModeEdit
		}
	}

	// Mouse control
	if rl.IsMouseButtonReleased(rl.MouseButtonLeft) {
		mousePos := rl.GetMousePosition()

		// change color
		for _, color := range data.colorPicker {
			if rl.CheckCollisionPointRec(mousePos, color.rec) {
				data.background = color.color
			}
		}

		// change speed
		if rl.CheckCollisionPointRec(mousePos, minusSpeedButton) {
			data.interval -= 0.01
		}

		if rl.CheckCollisionPointRec(mousePos, plusSpeedButton) {
			data.interval += 0.01
		}

		if rl.CheckCollisionPointRec(mousePos, playPauseButton) {
			if data.mode == ModeEdit {
				data.mode = ModePlay
			} else {
				data.mode = ModeEdit
			}
		}

		// Timeline selector
		offsetX := 10
		for index, texture := range data.textures {
			displayWidth := texture.Width
			displayHeight := texture.Height

			if displayWidth > 150 {
				displayWidth = 150
				displayHeight = texture.Height / int32(math.Ceil(float64(texture.Width)/150))
			}

			if displayHeight > 100 {
				displayHeight = 100
				displayWidth = texture.Width / int32(math.Ceil(float64(texture.Height)/100))
			}

			frameRec := rl.NewRectangle(float32(offsetX), 10, float32(displayWidth), float32(displayHeight))
			if rl.CheckCollisionPointRec(mousePos, frameRec) {
				data.frame = index
			}

			offsetX += int(displayWidth)
		}
	}

	if len(data.textures) > 0 {
		if data.frame >= len(data.textures) {
			data.frame = len(data.textures) - 1
		}
	}
}

func main() {
	rl.SetWindowState(rl.FlagMsaa4xHint + rl.FlagVsyncHint)
	rl.InitWindow(int32(screenWidth), int32(screenHeight), "AnimationTester")

	defer rl.CloseWindow()

	defaultFont := rl.GetFontDefault()

	if len(NotoSansFontData) > 0 {
		defaultFont = rl.LoadFontFromMemory(".ttf", NotoSansFontData, 20, nil)
	}

	data := Data{
		textures:   []rl.Texture2D{},
		interval:   0.2,
		background: rl.Gray,
		mode:       ModeEdit,
		fonts:      Fonts{defaultFont: defaultFont},
	}

	data.colorPicker = computeColorPicker()

	for !rl.WindowShouldClose() {
		control(&data)

		rl.BeginDrawing()
		rl.ClearBackground(rl.DarkGray)
		draw(&data)
		rl.EndDrawing()
	}
}
