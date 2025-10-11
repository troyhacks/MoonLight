# Modifiers

A modifier (💎) is an effect on an effect. It can change the size of the virtual layer, it can map the pixels other then 1:1 (e.g. mirror, multiply them or a 1D line can become a 2D circle) and it can change each light set in an effect during a loop (e.g. rotate the effect)

!!! tip

    Multiple effects and modifiers can be defined per layer. MoonLight supports unlimited number of them per layer. They will all run after each other, this is useful for modifiers as you can combine them: e.g. rotate then multiply creates four (or more) rotating effects.

## Modifier 💎 Nodes
🚧

### Pinwheel 💎💡

Projects 1D/2D effects onto 2D/3D fixtures in a pinwheel pattern.

* **Swirl** option to bend the pinwheel.
* **Reverse** option.
* **Rotation Symmetry**: Controls the rotational symmetry of the pattern.
* **Petals** option to adjust the number of petals.
* **Ztwist** option for 3D fixtures to twist the pattern along the z-axis.
* The virtual layer width is the number of petals. The height is the distance from center to corner

### RippleYZ 💎💡💫

Takes lights of an effect and copies them to other lights. E.g. 1D effect will be rippled to 2D, 2D effect will be rippled to 3D

* shrink: shrinks the original size to towardsY and Z
* towardsY: copies X into Y
* towardsZ: copies XY into Z