package net.ulno.gamenetcontroller.desktop;

import com.badlogic.gdx.backends.lwjgl3.Lwjgl3Application;
import com.badlogic.gdx.backends.lwjgl3.Lwjgl3ApplicationConfiguration;
import net.ulno.gamenetcontroller.Main;

public class DesktopLauncher {
	public static void main (String[] arg) {
		Lwjgl3ApplicationConfiguration config = new Lwjgl3ApplicationConfiguration();
		new Lwjgl3Application(new Main(), config);
	}
	/*
	//        LwjglApplicationConfiguration config = new LwjglApplicationConfiguration();
        Lwjgl3ApplicationConfiguration config = new Lwjgl3ApplicationConfiguration();
//        config.title = "McMinos-Mobile Desktop";
//        config.fullscreen = fs;
//        config.vSyncEnabled = true;
        config.setTitle("McMinos-Mobile Desktop");
        //config.fullscreen = fs;
        config.useVsync(true);
        if(!fs) {
            config.setWindowedMode(1280,900);
//            config.width = 1280;
//            config.height = 896;
//            config.width = 2048;
//            config.height = 1536;
        } else {
            DisplayMode mode = Lwjgl3ApplicationConfiguration.getDisplayMode();
            config.setFullscreenMode(mode);
//            config.width = config.getDesktopDisplayMode().width;
 //           config.height = config.getDesktopDisplayMode().height;
        }
        new Lwjgl3Application(new Main(), config);
	 */
}
