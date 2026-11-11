const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const programs = .{
        .{ "server", "src/server/server.cpp" },
        .{ "client", "src/client/client.cpp" },
    };

    inline for (programs) |program| {
        const module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
            .link_libcpp = true,
        });
        module.addCSourceFile(.{
            .file = b.path(program[1]),
            .flags = &.{ "-Wall", "-Wextra", "-pedantic" },
        });

        const exe = b.addExecutable(.{
            .name = "memvault-" ++ program[0],
            .root_module = module,
        });
        b.installArtifact(exe);

        const run = b.addRunArtifact(exe);
        const run_step = b.step("run-" ++ program[0], "Run the MemVault " ++ program[0]);
        run_step.dependOn(&run.step);
    }
}
