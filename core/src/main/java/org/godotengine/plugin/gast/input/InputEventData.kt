package org.godotengine.plugin.gast.input

internal sealed class InputEventData()

internal data class ActionEventData(val action: String, val pressed: Boolean, val strength: Float) :
    InputEventData()

internal data class HoverEventData(
    val nodePath: String,
    val pointerId: String,
    val xPercent: Float,
    val yPercent: Float
) : InputEventData()

internal data class PressEventData(
    val nodePath: String,
    val pointerId: String,
    val xPercent: Float,
    val yPercent: Float
) : InputEventData()

internal data class ReleaseEventData(
    val nodePath: String,
    val pointerId: String,
    val xPercent: Float,
    val yPercent: Float
) : InputEventData()

internal data class ScrollEventData(
    val nodePath: String,
    val pointerId: String,
    val xPercent: Float,
    val yPercent: Float,
    val horizontalDelta: Float,
    val verticalDelta: Float
) : InputEventData()
