{
  description = "The best minecraft launcher.";

  nixConfig = {
    substituters = [
      "https://cache.nixos.org"
      "https://cache.garnix.io"
      "https://HalkyLauncher.cachix.org"
    ];
    trusted-public-keys = [
      "cache.nixos.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY="
      "cache.garnix.io:CTFPyKSLcx5RMJKfLo5EEPUObbA78b0YQ2DTCJXqr9g="
      "HalkyLauncher.cachix.org-1:Jcp5Q9wiLL+EDv8Mh7c6L9xGk+lXr7/otpKxMOuBuDs="
    ];
  };

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    nix-filter.url = "github:numtide/nix-filter";
    libnbtplusplus = {
      url = "github:Oleksandr1811/libnbtplusplus";
      flake = false;
    };
  };

  outputs = {
    self,
    nixpkgs,
    nix-filter,
    libnbtplusplus,
    ...
  }: let
    systems = [
      "x86_64-linux"
      "aarch64-linux"
      "x86_64-darwin"
      "aarch64-darwin"
    ];

    forEachSystem = nixpkgs.lib.genAttrs systems;

    mkJvmPack = pkgs: let
      openjdk = with pkgs; [
        openjdk8
        openjdk17
        openjdk21
        openjdk25
      ];

      temurin = with pkgs.javaPackages.compiler.temurin-bin; [
        jdk-8
        jdk-17
        jdk-21
        jdk-25
      ];

      corretto = with pkgs.javaPackages.compiler; [
        corretto17
        corretto21
        corretto25
      ];

      graal-ce = with pkgs.graalvmPackages; [
        graalvm-ce
      ];

      graal-unfree = with pkgs.graalvmPackages; [
        graalvm-oracle_17
        graalvm-oracle_25
      ];

    in {
      inherit openjdk temurin corretto graal-ce graal-unfree;
      allPack = openjdk ++ temurin ++ corretto ++ graal-ce ++ graal-unfree;
    };

  in {
    overlays.default = final: prev: {
      HalkyLauncher-unwrapped = final.callPackage ./nix/unwrapped.nix {
        inherit nix-filter libnbtplusplus self;
      };

      HalkyLauncher = final.callPackage ./nix/wrapper.nix {
        jvmPack = mkJvmPack final;
      };
    };

    packages = forEachSystem (system: let
      pkgs = import nixpkgs {inherit system;};

      jvmPack = mkJvmPack pkgs;

      HalkyLauncher-unwrapped = pkgs.callPackage ./nix/unwrapped.nix {
        inherit nix-filter libnbtplusplus self;
      };

      HalkyLauncher = pkgs.callPackage ./nix/wrapper.nix {
        inherit HalkyLauncher-unwrapped jvmPack;
      };

      HalkyLauncher-unwrapped-debug = HalkyLauncher-unwrapped.overrideAttrs {
        cmakeBuildType = "Debug";
        dontStrip = true;
      };

      HalkyLauncher-debug = pkgs.callPackage ./nix/wrapper.nix {
        HalkyLauncher-unwrapped = HalkyLauncher-unwrapped-debug;
        inherit jvmPack;
      };
    in {
      inherit HalkyLauncher HalkyLauncher-unwrapped HalkyLauncher-debug HalkyLauncher-unwrapped-debug jvmPack;

      default = HalkyLauncher;
    });

    devShells = forEachSystem (system: let
      pkgs = import nixpkgs {
        inherit system;
        overlays = [self.overlays.default];
      };
    in {
      default = pkgs.mkShell {
        inputsFrom = [pkgs.HalkyLauncher-unwrapped];

        packages = with pkgs; [
          ccache
          ninja
        ];
      };
    });

    formatter = forEachSystem (
      system:
        (import nixpkgs {inherit system;}).alejandra
    );
  };
}
