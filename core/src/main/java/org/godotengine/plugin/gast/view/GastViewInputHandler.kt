package org.godotengine.plugin.gast.view

import android.os.SystemClock
import android.view.InputDevice
import android.view.MotionEvent
import androidx.collection.ArraySet
import org.godotengine.plugin.gast.input.GastInputListener
import kotlin.math.max
import kotlin.math.min

/**
 * Utility class used to handle common input related logic.
 */
internal class GastViewInputHandler(private val gastView: GastFrameLayout) : GastInputListener {

    companion object {
        /**
         * Required for hover motion events to be properly handled.
         */
        private const val HOVER_INPUT_SOURCE = InputDevice.SOURCE_CLASS_POINTER

        /**
         * Multiplier to convert the scrolling speed of controller to the speed in Android
         * scroll event.
         */
        private const val SCROLL_SENSITIVITY = 1.6F

        /**
         * Upper bound of the speed in the scrolling event.
         */
        private const val SCROLL_SPEED_LIMIT = 0.16f
    }

    /**
     * Keep tracks of whether each pointer is in the HOVER state.
     */
    private val hoverEnteredSet = HashSet<String>()

    /**
     * Keep tracks of whether an ACTION_DOWN gesture has occurred for each pointer.
     */
    private val onDownSet = HashSet<String>()

    private val pointerIdsTracker = ArraySet<String>(5)

    private fun getScrollByDelta(delta: Float) =
        max(-SCROLL_SPEED_LIMIT, min(SCROLL_SPEED_LIMIT, SCROLL_SENSITIVITY * delta))

    private fun obtainMotionEvent(
        pointerId: String,
        eventTime: Long,
        action: Int,
        source: Int,
        xCoord: Float = 0F,
        yCoord: Float = 0F
    ): MotionEvent {
        val pointerProperties = arrayOf(MotionEvent.PointerProperties().apply {
            this.id = getMotionEventPointerId(pointerId)
        })
        val pointerCoords = arrayOf(MotionEvent.PointerCoords().apply {
            this.x = xCoord
            this.y = yCoord
            this.pressure = 1.0f
            this.size = 1.0f
        })
        return MotionEvent.obtain(
            eventTime,
            eventTime,
            action,
            1,
            pointerProperties,
            pointerCoords,
            0,
            0,
            1.0f,
            1.0f,
            0,
            0,
            source,
            0
        )
    }

    private fun obtainScrollEvent(
        pointerId: String,
        eventTime: Long,
        xCoord: Float,
        yCoord: Float,
        scrollByX: Float,
        scrollByY: Float
    ): MotionEvent {
        val pointerProperties = arrayOf(MotionEvent.PointerProperties().apply {
            this.id = getMotionEventPointerId(pointerId)
        })
        val pointerCoords = arrayOf(MotionEvent.PointerCoords().apply {
            this.setAxisValue(MotionEvent.AXIS_X, xCoord)
            this.setAxisValue(MotionEvent.AXIS_Y, yCoord)
            this.setAxisValue(MotionEvent.AXIS_HSCROLL, scrollByX)
            this.setAxisValue(MotionEvent.AXIS_VSCROLL, -scrollByY)
        })
        return MotionEvent.obtain(
            eventTime,
            eventTime,
            MotionEvent.ACTION_SCROLL,
            1,
            pointerProperties,
            pointerCoords,
            0,
            0,
            1.0f,
            1.0f,
            0,
            0,
            InputDevice.SOURCE_CLASS_POINTER,
            0
        )
    }

    override fun onMainInputHover(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        if (nodePath.isBlank() || nodePath != gastView.gastNode?.nodePath) {
            return
        }

        val eventTime = SystemClock.uptimeMillis()

        // xPercent and yPercent being less than 0 indicates this is no longer being looked at.
        if (!onDownSet.contains(pointerId) && xPercent < 0 && yPercent < 0) {
            if (hoverEnteredSet.contains(pointerId)) {
                val motionEvent = obtainMotionEvent(
                    pointerId,
                    eventTime,
                    MotionEvent.ACTION_HOVER_EXIT,
                    HOVER_INPUT_SOURCE
                )
                gastView.dispatchGenericMotionEvent(motionEvent)
                hoverEnteredSet.remove(pointerId)
                motionEvent.recycle()
            }
        } else {
            val xCoord = xPercent * gastView.width
            val yCoord = yPercent * gastView.height
            if (onDownSet.contains(pointerId)) {
                // Send a ACTION_MOVE event.
                val motionEvent = obtainMotionEvent(
                    pointerId,
                    eventTime,
                    MotionEvent.ACTION_MOVE,
                    InputDevice.SOURCE_TOUCHSCREEN,
                    xCoord,
                    yCoord
                )
                gastView.dispatchTouchEvent(motionEvent)
                motionEvent.recycle()
            } else {
                // Send a hover event.
                val action =
                    if (hoverEnteredSet.contains(pointerId)) MotionEvent.ACTION_HOVER_MOVE else MotionEvent.ACTION_HOVER_ENTER
                val motionEvent =
                    obtainMotionEvent(
                        pointerId,
                        eventTime,
                        action,
                        HOVER_INPUT_SOURCE,
                        xCoord,
                        yCoord
                    )
                gastView.dispatchGenericMotionEvent(motionEvent)
                hoverEnteredSet.add(pointerId)
                motionEvent.recycle()
            }
        }
    }

    override fun onMainInputPress(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        if (nodePath.isBlank() || nodePath != gastView.gastNode?.nodePath) {
            return
        }

        val eventTime = SystemClock.uptimeMillis()
        val xCoord = xPercent * gastView.width
        val yCoord = yPercent * gastView.height

        if (hoverEnteredSet.contains(pointerId)) {
            // Complete the hover motion event.
            val motionEvent = obtainMotionEvent(
                pointerId,
                eventTime,
                MotionEvent.ACTION_HOVER_EXIT,
                HOVER_INPUT_SOURCE,
                xCoord,
                yCoord
            )
            gastView.dispatchGenericMotionEvent(motionEvent)
            hoverEnteredSet.remove(pointerId)
            motionEvent.recycle()
        }

        // Send a ACTION_DOWN motion event.
        val motionEvent = obtainMotionEvent(
            pointerId,
            eventTime,
            MotionEvent.ACTION_DOWN,
            InputDevice.SOURCE_TOUCHSCREEN,
            xCoord,
            yCoord
        )
        gastView.dispatchTouchEvent(motionEvent)
        onDownSet.add(pointerId)
        motionEvent.recycle()
    }

    override fun onMainInputRelease(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        if (nodePath.isBlank() || nodePath != gastView.gastNode?.nodePath) {
            return
        }

        if (!onDownSet.contains(pointerId)) {
            return
        }

        val eventTime = SystemClock.uptimeMillis()
        val xCoord = xPercent * gastView.width
        val yCoord = yPercent * gastView.height

        // Send a ACTION_UP motion event to complete the gesture.
        val motionEvent = obtainMotionEvent(
            pointerId,
            eventTime,
            MotionEvent.ACTION_UP,
            InputDevice.SOURCE_TOUCHSCREEN,
            xCoord,
            yCoord
        )
        gastView.dispatchTouchEvent(motionEvent)
        onDownSet.remove(pointerId)
        motionEvent.recycle()
    }

    override fun onMainInputScroll(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float,
        horizontalDelta: Float,
        verticalDelta: Float
    ) {
        if (nodePath.isBlank() || nodePath != gastView.gastNode?.nodePath) {
            return
        }

        val eventTime = SystemClock.uptimeMillis()
        val xCoord = xPercent * gastView.width
        val yCoord = yPercent * gastView.height

        val scrollByX = getScrollByDelta(horizontalDelta)
        val scrollByY = getScrollByDelta(verticalDelta)

        val motionEvent =
            obtainScrollEvent(pointerId, eventTime, xCoord, yCoord, scrollByX, scrollByY)
        gastView.dispatchGenericMotionEvent(motionEvent)
        motionEvent.recycle()
    }

    private fun getMotionEventPointerId(godotPointerId: String): Int {
        pointerIdsTracker.add(godotPointerId)
        return pointerIdsTracker.indexOf(godotPointerId)
    }
}
