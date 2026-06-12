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
  in {
    overlays.default = final: prev: {
      HalkyLauncher-unwrapped = final.callPackage ./nix/unwrapped.nix {
        inherit nix-filter libnbtplusplus self;
      };

      HalkyLauncher = final.callPackage ./nix/wrapper.nix;
    };

    packages = forEachSystem (system: let
      pkgs = import nixpkgs {inherit system;};

      HalkyLauncher-unwrapped = pkgs.callPackage ./nix/unwrapped.nix {
        inherit nix-filter libnbtplusplus self;
      };

      HalkyLauncher = pkgs.callPackage ./nix/wrapper.nix {
        inherit HalkyLauncher-unwrapped;
      };

      HalkyLauncher-unwrapped-debug = HalkyLauncher-unwrapped.overrideAttrs {
        cmakeBuildType = "Debug";
        dontStrip = true;
      };

      HalkyLauncher-debug = pkgs.callPackage ./nix/wrapper.nix {
        HalkyLauncher-unwrapped = HalkyLauncher-unwrapped-debug;
      };
    in {
      inherit HalkyLauncher HalkyLauncher-unwrapped HalkyLauncher-debug HalkyLauncher-unwrapped-debug;

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
