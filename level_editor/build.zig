const std = @import("std");
const raylib = @import("./lib/raylib/src/build.zig");

// Although this function looks imperative, note that its job is to
// declaratively construct a build graph that will be executed by an external
// runner.
pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const lib = raylib.addRaylib(b, target, optimize, .{});

    const exe = b.addExecutable(.{
        .name = "level_editor",
        .root_source_file = .{ .path = "./main.cpp" },
        .target = target,
        .optimize = optimize,
    });

    exe.linkLibrary(lib);
    const raylib_flags = &[_][]const u8{
        "-D_GNU_SOURCE",
        "-DGL_SILENCE_DEPRECATION=199309L",
        "-fno-sanitize=undefined", // https://github.com/raysan5/raylib/issues/1891,
    };
    exe.linkSystemLibrary("c++");

    const sources = [_][]const u8{
        // Pretty surprised that this is not required.
        // "./window/block_type_editor.cpp",
        // "./window/exit.cpp",
        // "./window/export.cpp",
        // "./window/key_value_editor.cpp",
    };

    const includeDirs = [_][]const u8{
        "./lib/raylib/src/",
        //"./lib/raygui/src/",
        // "./export/",
        // "./interfaces/",
        // "./window/",
    };

    inline for (includeDirs) |incl_dir| {
        exe.addIncludePath(.{ .path = incl_dir });
    }
    inline for (sources) |src| {
        exe.addCSourceFile(.{ .file = .{ .path = src }, .flags = raylib_flags });
    }

    // This declares intent for the executable to be installed into the
    // standard location when the user invokes the "install" step (the default
    // step when running `zig build`).
    b.installArtifact(exe);

    // This *creates* a Run step in the build graph, to be executed when another
    // step is evaluated that depends on it. The next line below will establish
    // such a dependency.
    const run_cmd = b.addRunArtifact(exe);

    // By making the run step depend on the install step, it will be run from the
    // installation directory rather than directly from within the cache directory.
    // This is not necessary, however, if the application depends on other installed
    // files, this ensures they will be present and in the expected location.
    run_cmd.step.dependOn(b.getInstallStep());

    // This allows the user to pass arguments to the application in the build
    // command itself, like this: `zig build run -- arg1 arg2 etc`
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    // This creates a build step. It will be visible in the `zig build --help` menu,
    // and can be selected like this: `zig build run`
    // This will evaluate the `run` step rather than the default, which is "install".
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    // Creates a step for unit testing. This only builds the test executable
    // but does not run it.
    const unit_tests = b.addTest(.{
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_unit_tests = b.addRunArtifact(unit_tests);

    // Similar to creating the run step earlier, this exposes a `test` step to
    // the `zig build --help` menu, providing a way for the user to request
    // running the unit tests.
    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_unit_tests.step);
}
