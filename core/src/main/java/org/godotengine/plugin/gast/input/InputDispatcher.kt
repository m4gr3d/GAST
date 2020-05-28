package org.godotengine.plugin.gast.input

import android.util.Log
import androidx.core.util.Pools
import java.util.concurrent.ConcurrentLinkedQueue

internal class InputDispatcher private constructor() : Runnable {

    companion object {
        private const val POOL_MAX_SIZE = 100
        private val TAG = InputDispatcher::class.java.simpleName

        private val inputDispatcherPool = Pools.SynchronizedPool<InputDispatcher>(POOL_MAX_SIZE)

        fun acquireInputDispatcher(
            gastInputListeners: ConcurrentLinkedQueue<GastInputListener>,
            inputType: InputType,
            nodePath: String,
            pointerId: String,
            inputData: FloatArray
        ): InputDispatcher {
            val dispatcher = inputDispatcherPool.acquire() ?: InputDispatcher()
            dispatcher.apply {
                this.gastInputListeners = gastInputListeners
                this.inputType = inputType
                this.nodePath = nodePath
                this.pointerId = pointerId
                this.inputData = inputData
            }

            return dispatcher
        }

        fun releaseInputDispatcher(dispatcher: InputDispatcher) {
            if (!inputDispatcherPool.release(dispatcher)) {
                Log.w(TAG, "Input dispatcher pool reached its size limit (${POOL_MAX_SIZE})!")
            }
        }
    }

    lateinit var gastInputListeners: ConcurrentLinkedQueue<GastInputListener>
    lateinit var inputType: InputType
    lateinit var nodePath: String
    lateinit var pointerId: String
    lateinit var inputData: FloatArray

    override fun run() {

        when (inputType) {
            InputType.HOVER -> {
                for (listener in gastInputListeners) {
                    listener.onMainInputHover(nodePath, pointerId, inputData[0], inputData[1])
                }
            }

            InputType.PRESS -> {
                for (listener in gastInputListeners) {
                    listener.onMainInputPress(nodePath, pointerId, inputData[0], inputData[1])
                }
            }

            InputType.RELEASE -> {
                for (listener in gastInputListeners) {
                    listener.onMainInputRelease(nodePath, pointerId, inputData[0], inputData[1])
                }
            }

            InputType.SCROLL -> {
                for (listener in gastInputListeners) {
                    listener.onMainInputScroll(
                        nodePath,
                        pointerId,
                        inputData[0],
                        inputData[1],
                        inputData[2],
                        inputData[3]
                    )
                }
            }
        }

        releaseInputDispatcher(this)
    }
}
