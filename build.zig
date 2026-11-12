const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const programs = .{
        .{ .name = "server", .sources = &.{"src/server/server.cpp"} },
        .{ .name = "client", .sources = &.{"src/client/client.cpp"} },
    };

    inline for (programs) |program| {
        const module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
            .link_libcpp = true,
        });
        module.addCSourceFiles(.{
            .files = program.sources,
            .flags = &.{ "-Wall", "-Wextra", "-pedantic" },
        });

        const exe = b.addExecutable(.{
            .name = "memvault-" ++ program.name,
            .root_module = module,
        });
        b.installArtifact(exe);

        const run = b.addRunArtifact(exe);
        const run_step = b.step("run-" ++ program.name, "Run the MemVault " ++ program.name);
        run_step.dependOn(&run.step);
    }
}
