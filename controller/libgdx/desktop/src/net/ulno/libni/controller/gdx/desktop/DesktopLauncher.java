package net.ulno.libni.controller.gdx.desktop;

import com.badlogic.gdx.Graphics;
import com.badlogic.gdx.Graphics.DisplayMode;
import com.badlogic.gdx.backends.lwjgl.LwjglApplication;
import com.badlogic.gdx.backends.lwjgl.LwjglApplicationConfiguration;
import net.ulno.libni.controller.gdx.Main;

public class DesktopLauncher {
	public static void main (String[] arg) {
		LwjglApplicationConfiguration config = new LwjglApplicationConfiguration();
        config.title = "Libni Libgdx Controller";
        config.width = 1280;
        config.height = 900;
		new LwjglApplication(new Main(), config);
	}
}
