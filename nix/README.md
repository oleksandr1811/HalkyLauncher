# NixOS

> [!IMPORTANT]
> We use **Garnix CI** for binary caching.
> To configure the cache manually, follow the official guide:
>
> - [Garnix binary cache guide](https://garnix.io/docs/ci/caching)
>
> We also provide a secondary cache through [**Cachix**](https://app.cachix.org/cache/HalkyLauncher#pull).
> Additional information is available in the official
> [Cachix getting started guide](https://docs.cachix.org/getting-started#using-binaries-with-nix).

<div align="center">

# Running and installing on NixOS

This guide explains how to run and install **HalkyLauncher** on NixOS.

</div>

## Running without installation

```fish
nix run github:Oleksandr1811/HalkyLauncher#HalkyLauncher
```

## Installation

Add the flake input to your `flake.nix`:

```nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    HalkyLauncher = {
      url = "github:Oleksandr1811/HalkyLauncher";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, HalkyLauncher, ... }: {
    # your outputs
  };
}
```

### NixOS configuration

```nix
{ pkgs, system, HalkyLauncher, ... }:

{
  environment.systemPackages = [
    HalkyLauncher.packages.${system}.HalkyLauncher
  ];
}
```

### Home Manager configuration

```nix
{ pkgs, system, HalkyLauncher, ... }:

{
  home.packages = [
    HalkyLauncher.packages.${system}.HalkyLauncher
  ];
}
```

## Updating

To update the flake input:

```fish
nix flake update HalkyLauncher
```

Or update all inputs:

```fish
nix flake update
```
