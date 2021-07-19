package org.godotengine.plugin.gast.input

internal sealed class InputEventData(
    val nodePath: String,
    val pointerId: String,
    val xPercent: Float,
    val yPercent: Float
)

internal class HoverEventData(
    nodePath: String,
    pointerId: String,
    xPercent: Float,
    yPercent: Float
) : InputEventData(nodePath, pointerId, xPercent, yPercent)

internal class PressEventData(
    nodePath: String,
    pointerId: String,
    xPercent: Float,
    yPercent: Float
) : InputEventData(nodePath, pointerId, xPercent, yPercent)

internal class ReleaseEventData(
    nodePath: String,
    pointerId: String,
    xPercent: Float,
    yPercent: Float
) : InputEventData(nodePath, pointerId, xPercent, yPercent)

internal class ScrollEventData(
    nodePath: String,
    pointerId: String,
    xPercent: Float,
    yPercent: Float,
    val horizontalDelta: Float,
    val verticalDelta: Float
) : InputEventData(nodePath, pointerId, xPercent, yPercent)
