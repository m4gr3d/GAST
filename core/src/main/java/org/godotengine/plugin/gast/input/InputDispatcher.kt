package org.godotengine.plugin.gast.input

import android.util.Log
import androidx.core.util.Pools
import java.util.concurrent.ConcurrentLinkedQueue

internal class InputDispatcher(
    var gastInputListeners: ConcurrentLinkedQueue<GastInputListener>,
    var inputType: InputType,
    var nodePath: String,
    var pointerId: String,
    var xPercent: Float,
    var yPercent: Float
) : Runnable {

    companion object {
        private const val POOL_MAX_SIZE = 100
        private val TAG = InputDispatcher::class.java.simpleName

        private val inputDispatcherPool = Pools.SynchronizedPool<InputDispatcher>(POOL_MAX_SIZE)

        fun acquireInputDispatcher(
            gastInputListeners: ConcurrentLinkedQueue<GastInputListener>,
            inputType: InputType,
            nodePath: String,
            pointerId: String,
            xPercent: Float,
            yPercent: Float
        ): InputDispatcher {
            var dispatcher = inputDispatcherPool.acquire()
            if (dispatcher == null) {
                dispatcher = InputDispatcher(
                    gastInputListeners,
                    inputType,
                    nodePath,
                    pointerId,
                    xPercent,
                    yPercent
                )
            } else {
                dispatcher.apply {
                    this.gastInputListeners = gastInputListeners
                    this.inputType = inputType
                    this.nodePath = nodePath
                    this.pointerId = pointerId
                    this.xPercent = xPercent
                    this.yPercent = yPercent
                }
            }

            return dispatcher
        }

        fun releaseInputDispatcher(dispatcher: InputDispatcher) {
            if (!inputDispatcherPool.release(dispatcher)) {
                Log.w(TAG, "Input dispatcher pool reached its size limit (${POOL_MAX_SIZE})!")
            }
        }
    }

    override fun run() {

        when (inputType) {
            InputType.HOVER -> {
                for (listener in gastInputListeners) {
                    listener.onMainInputHover(nodePath, pointerId, xPercent, yPercent)
                }
            }

            InputType.PRESS -> {
                for (listener in gastInputListeners) {
                    listener.onMainInputPress(nodePath, pointerId, xPercent, yPercent)
                }
            }

            InputType.RELEASE -> {
                for (listener in gastInputListeners) {
                    listener.onMainInputRelease(nodePath, pointerId, xPercent, yPercent)
                }
            }
        }

        releaseInputDispatcher(this)
    }
}
