package org.godotengine.plugin.gast.input.action

import android.util.Log
import androidx.core.util.Pools
import java.util.Queue

internal class InputActionDispatcher private constructor(): Runnable {

    companion object {
        private const val POOL_MAX_SIZE = 1000
        private val TAG = InputActionDispatcher::class.java.simpleName

        private val inputActionDispatcherPool = Pools.SynchronizedPool<InputActionDispatcher>(POOL_MAX_SIZE)

        fun acquireInputActionDispatcher(
            gastActionListeners: Queue<GastActionListener>,
            action: String,
            pressState: GastActionListener.InputPressState,
            strength: Float
        ): InputActionDispatcher {
            val dispatcher = inputActionDispatcherPool.acquire() ?: InputActionDispatcher()
            dispatcher.apply {
                this.gastActionListeners = gastActionListeners
                this.action = action
                this.pressState = pressState
                this.strength = strength
            }

            return dispatcher
        }

        fun releaseInputDispatcher(dispatcher: InputActionDispatcher) {
            if (!inputActionDispatcherPool.release(dispatcher)) {
                Log.w(TAG, "Input action dispatcher pool reached its size limit ($POOL_MAX_SIZE)!")
            }
        }
    }

    lateinit var gastActionListeners: Queue<GastActionListener>
    lateinit var action: String
    lateinit var pressState: GastActionListener.InputPressState
    var strength: Float = 0f

    override fun run() {
        for (listener in gastActionListeners) {
            listener.onMainInputAction(
                action,
                pressState,
                strength
            )
        }

        releaseInputDispatcher(this)
    }
}
