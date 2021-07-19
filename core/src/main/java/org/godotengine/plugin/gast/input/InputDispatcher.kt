package org.godotengine.plugin.gast.input

import android.util.Log
import androidx.core.util.Pools
import java.util.Queue

internal class InputDispatcher private constructor() : Runnable {

    companion object {
        private const val POOL_MAX_SIZE = 1000
        private val TAG = InputDispatcher::class.java.simpleName

        private val inputDispatcherPool = Pools.SynchronizedPool<InputDispatcher>(POOL_MAX_SIZE)

        fun acquireInputDispatcher(
            gastInputListeners: Queue<GastInputListener>,
            eventData: InputEventData
        ): InputDispatcher {
            val dispatcher = inputDispatcherPool.acquire() ?: InputDispatcher()
            dispatcher.apply {
                this.gastInputListeners = gastInputListeners
                this.eventData = eventData
            }

            return dispatcher
        }

        fun releaseInputDispatcher(dispatcher: InputDispatcher) {
            if (!inputDispatcherPool.release(dispatcher)) {
                Log.w(TAG, "Input dispatcher pool reached its size limit (${POOL_MAX_SIZE})!")
            }
        }
    }

    lateinit var gastInputListeners: Queue<GastInputListener>
    lateinit var eventData: InputEventData

    override fun run() {

        when (eventData) {

            is HoverEventData -> {
                val hoverEventData = eventData as HoverEventData
                for (listener in gastInputListeners) {
                    listener.onMainInputHover(
                        hoverEventData.nodePath,
                        hoverEventData.pointerId,
                        hoverEventData.xPercent,
                        hoverEventData.yPercent
                    )
                }
            }

            is PressEventData -> {
                val pressEventData = eventData as PressEventData
                for (listener in gastInputListeners) {
                    listener.onMainInputPress(
                        pressEventData.nodePath,
                        pressEventData.pointerId,
                        pressEventData.xPercent,
                        pressEventData.yPercent
                    )
                }
            }

            is ReleaseEventData -> {
                val releaseEventData = eventData as ReleaseEventData
                for (listener in gastInputListeners) {
                    listener.onMainInputRelease(
                        releaseEventData.nodePath,
                        releaseEventData.pointerId,
                        releaseEventData.xPercent,
                        releaseEventData.yPercent
                    )
                }
            }

            is ScrollEventData -> {
                val scrollEventData = eventData as ScrollEventData
                for (listener in gastInputListeners) {
                    listener.onMainInputScroll(
                        scrollEventData.nodePath,
                        scrollEventData.pointerId,
                        scrollEventData.xPercent,
                        scrollEventData.yPercent,
                        scrollEventData.horizontalDelta,
                        scrollEventData.verticalDelta
                    )
                }
            }
        }

        releaseInputDispatcher(this)
    }
}
