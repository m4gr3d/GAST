package org.godotengine.plugin.gast.video

import android.app.Activity
import android.graphics.SurfaceTexture
import android.text.TextUtils
import android.util.Log
import android.view.Surface
import android.view.View
import com.google.android.exoplayer2.ExoPlaybackException
import com.google.android.exoplayer2.Player
import com.google.android.exoplayer2.SimpleExoPlayer
import com.google.android.exoplayer2.source.ConcatenatingMediaSource
import com.google.android.exoplayer2.source.ProgressiveMediaSource
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory
import com.google.android.exoplayer2.upstream.RawResourceDataSource
import com.google.android.exoplayer2.util.Util
import org.godotengine.plugin.gast.GastManager
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.GodotPluginRegistry
import org.godotengine.plugin.gast.input.GastInputListener
import org.godotengine.plugin.gast.GastRenderListener
import java.util.concurrent.atomic.AtomicBoolean
import java.util.concurrent.atomic.AtomicInteger

/**
 * GAST video plugin
 *
 * This plugin is used to play audio/video contents in the Godot engine.
 * It's powered by Exoplayer and thus support a wide range of codecs.
 */
class GastVideoPlugin(godot: Godot) : GodotPlugin(godot), Player.EventListener,
    SurfaceTexture.OnFrameAvailableListener, GastRenderListener, GastInputListener {

    companion object {
        private val TAG = GastVideoPlugin::class.java.simpleName
    }

    private val gastManager: GastManager by lazy {
        GodotPluginRegistry.getPluginRegistry().getPlugin("gast-core") as GastManager
    }
    private val player = SimpleExoPlayer.Builder(godot).build()

    private val updateTextureImageCounter = AtomicInteger()
    private val playing = AtomicBoolean(false)

    private var parentNodePath = ""
    private var videoNodePath = ""
    private var surfaceTexture: SurfaceTexture? = null

    init {
        player.addListener(this)
    }

    override fun getPluginMethods() = mutableListOf(
        "preparePlayer",
        "play",
        "isPlaying",
        "pause",
        "seekTo",
        "stop",
        "setVideoScreenSize",
        "setRepeatMode"
    )

    override fun getPluginName() = "gast-video"

    override fun onMainCreateView(activity: Activity): View? {
        gastManager.addGastEventListener(this)
        gastManager.addGastInputListener(this)
        return null
    }

    override fun onMainDestroy() {
        gastManager.removeGastEventListener(this)
        gastManager.removeGastInputListener(this)
        player.release()
        surfaceTexture?.release()
    }

    override fun onIsPlayingChanged(isPlaying: Boolean) {
        playing.set(isPlaying)
    }

    override fun onPlayerError(error: ExoPlaybackException) {
        Log.e(TAG, error.message, error)
    }

    override fun onFrameAvailable(surfaceTexture: SurfaceTexture) {
        updateTextureImageCounter.incrementAndGet()
    }

    override fun onRenderDrawFrame() {
        var counter = updateTextureImageCounter.get()
        while (counter > 0) {
            surfaceTexture?.updateTexImage()
            counter = updateTextureImageCounter.decrementAndGet()
        }
    }

    override fun onPositionDiscontinuity(@Player.DiscontinuityReason reason: Int) {
        when (reason) {

            Player.DISCONTINUITY_REASON_PERIOD_TRANSITION, Player.DISCONTINUITY_REASON_SEEK -> {
                // TODO: use to detect when switching video in a playlist
            }
            else -> {
            }
        }
    }

    private fun isInitialized() = !TextUtils.isEmpty(videoNodePath)

    private fun setVideoNodePath(parentNodePath: String) {
        if (TextUtils.isEmpty(parentNodePath)) {
            Log.e(TAG, "Invalid parent node path value: $parentNodePath")
            return
        }

        if (parentNodePath == this.parentNodePath) {
            // Nothing to do since it's the same parent node.
            return
        }

        runOnRenderThread {
            // Reset the current node.
            if (!TextUtils.isEmpty(videoNodePath)) {
                gastManager.unbindAndReleaseGastNode(videoNodePath)
                runOnUiThread {
                    player.clearVideoSurface()
                    surfaceTexture?.release()
                    surfaceTexture = null
                }
            }

            // Setup the new node
            videoNodePath = gastManager.acquireAndBindGastNode(parentNodePath)
            val texId = gastManager.getExternalTextureId(videoNodePath)
            this.parentNodePath = parentNodePath

            runOnUiThread {
                surfaceTexture = SurfaceTexture(texId)
                surfaceTexture?.setOnFrameAvailableListener(this)
                val surface = Surface(surfaceTexture)
                player.setVideoSurface(surface)
            }
        }
    }

    private fun setVideoSource(videoRawNames: Array<String>) {
        if (videoRawNames.isEmpty()) {
            Log.e(TAG, "No available videos to load: $videoRawNames")
            return
        }

        runOnUiThread {
            val resources = godot.resources
            val packageName = godot.packageName
            val dataSourceFactory =
                DefaultDataSourceFactory(godot, Util.getUserAgent(godot, pluginName))
            val videosSource = ConcatenatingMediaSource()

            for (videoRawName in videoRawNames) {
                val videoRawId = resources.getIdentifier(videoRawName, "raw", packageName)
                if (videoRawId == 0) {
                    Log.e(
                        TAG,
                        "Unable to find video $videoRawName in the resources 'raw' directory."
                    )
                    continue
                }

                val uri = RawResourceDataSource.buildRawResourceUri(videoRawId)
                val source =
                    ProgressiveMediaSource.Factory(dataSourceFactory)
                        .setTag(videoRawId)
                        .createMediaSource(uri)
                videosSource.addMediaSource(source)
            }
            player.prepare(videosSource)
        }
    }

    fun preparePlayer(parentNodePath: String, videoRawNames: Array<String>) {
        setVideoNodePath(parentNodePath)
        setVideoSource(videoRawNames)
    }

    fun setVideoScreenSize(width: Float, height: Float) {
        runOnRenderThread {
            gastManager.updateGastNodeSize(videoNodePath, width, height)
        }
    }

    fun play() {
        runOnUiThread {
            player.playWhenReady = true
        }

    }

    fun isPlaying() = isInitialized() && playing.get()

    fun pause() {
        runOnUiThread {
            player.playWhenReady = false
        }
    }

    fun setRepeatMode(repeatMode: Int) {
        runOnUiThread {
            player.repeatMode = repeatMode
        }
    }

    fun seekTo(positionInMsec: Int) {
        runOnUiThread {
            player.seekTo(positionInMsec.toLong())
        }
    }

    fun stop() {
        runOnUiThread {
            player.stop()
        }
    }

    override fun onMainInputHover(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {

    }

    override fun onMainInputPress(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {

    }

    override fun onMainInputRelease(
        nodePath: String,
        pointerId: String,
        xPercent: Float,
        yPercent: Float
    ) {
        if (nodePath == videoNodePath) {
            if (isPlaying()) {
                pause()
            } else {
                play()
            }
        }
    }

}
