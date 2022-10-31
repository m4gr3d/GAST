package org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.videos

import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import com.google.android.exoplayer2.PlaybackException
import com.google.android.exoplayer2.Player
import org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer.R

/**
 * Sets up the controls used to manager video playback.
 */
class VideoControlsFragment : VideoFragment(), Player.Listener {

    companion object {
        private val TAG = VideoControlsFragment::class.java.simpleName
        private const val HOVER_ELEVATION_OFFSET = 20
    }

    private val playPauseButton: ImageView by lazy {
        requireView().findViewById(R.id.media_play_pause_control)
    }
    private val playPauseButtonContainer: View by lazy {
        requireView().findViewById(R.id.media_play_pause_control_container)
    }

    private val nextButton: View by lazy {
        requireView().findViewById(R.id.media_next_control)
    }
    private val nextButtonContainer: View by lazy {
        requireView().findViewById(R.id.media_next_control_container)
    }

    private val previousButton: View by lazy {
        requireView().findViewById(R.id.media_prev_control)
    }
    private val previousButtonContainer: View by lazy {
        requireView().findViewById(R.id.media_prev_control_container)
    }


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        bundle: Bundle?
    ): View? {
        return inflater.inflate(R.layout.video_controls_fragment, container, false)
    }

    override fun onViewCreated(view: View, bundle: Bundle?) {
        super.onViewCreated(view, bundle)

        val videoControls = view.findViewById<View>(R.id.video_controls)
        if (isXrEnabled()) {
            videoControls.rotationX = -20f
        }

        getPlayer()?.addListener(this@VideoControlsFragment)

        playPauseButton.setOnClickListener {
            if (getPlayer()?.isPlaying == true) {
                pause()
            } else {
                play()
            }
        }
        playPauseButton.setOnHoverListener { v, event ->
            onControlHover(
                playPauseButtonContainer,
                event
            )
        }

        nextButton.setOnClickListener { next() }
        nextButton.setOnHoverListener { v, event -> onControlHover(nextButtonContainer, event) }

        previousButton.setOnClickListener { previous() }
        previousButton.setOnHoverListener { v, event ->
            onControlHover(
                previousButtonContainer,
                event
            )
        }
    }

    override fun onPause() {
        super.onPause()

        getPlayer()?.apply {
            if (isPlaying) {
                pause()
            }
        }
    }

    private fun play() {
        getPlayer()?.apply {
            playWhenReady = true
        }
    }

    private fun pause() {
        getPlayer()?.apply {
            playWhenReady = false
        }
    }

    private fun next() {
        getPlayer()?.apply {
            seekToNextWindow()
        }
    }

    private fun previous() {
        getPlayer()?.apply {
            seekToPreviousWindow()
        }
    }

    override fun onIsPlayingChanged(isPlaying: Boolean) {
        val controlResId =
            if (isPlaying) android.R.drawable.ic_media_pause else android.R.drawable.ic_media_play
        playPauseButton.setImageResource(controlResId)
    }

    override fun onPlayerError(error: PlaybackException) {
        Log.e(TAG, "Player error occurred", error)
    }

    override fun onPlayerErrorChanged(error: PlaybackException?) {
        Log.e(TAG, "Player error changed", error)
    }

    private fun onControlHover(view: View, event: MotionEvent): Boolean {
        return when (event.actionMasked) {
            MotionEvent.ACTION_HOVER_ENTER -> {
                view.elevation = view.elevation + HOVER_ELEVATION_OFFSET
                true
            }
            MotionEvent.ACTION_HOVER_EXIT -> {
                view.elevation = view.elevation - HOVER_ELEVATION_OFFSET
                true
            }
            else -> false
        }
    }
}
