func zigExporter() -> ExportConfig {
    return ExportConfig(
        id: "zig",
        name: "Zig",
        ext: "zig",
        generate: { editor in
            var out = "const rl = @import(\"raylib_base.zig\");\n\n"
            out.append("const \(editor.levelName): [\(editor.objects.count)]@import(\"types.zig\").Platform  = .{\n")
            for item in editor.objects {
                out.append(".{.rectangle = .{.x = \(item.x), .y = \(item.y), .width = \(item.width), .height = \(item.height) }, .platformType = TODO,},\n")
            }
            out.append("};\n")
            return out
        }
    )
}
