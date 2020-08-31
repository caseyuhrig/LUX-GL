#pragma once


/*
   std::string soundBackgroundFilename = "res/audio/tlsf/engineloop.wav";
   std::string soundFilename = "res/audio/tlsf/Laser1.wav";
   irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

   auto sound_camera_location = irrklang::vec3df(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
   auto sound_camera_look_at = irrklang::vec3df(camera.GetLookAt().x, camera.GetLookAt().y, camera.GetLookAt().z);

   engine->setListenerPosition(sound_camera_location, sound_camera_look_at);
   engine->setSoundVolume(1.0f);
   engine->setDefault3DSoundMinDistance(10.0f); // [default] 1.0f

   auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
   irrklang::ISound* sound1 = NULL;
   */


   /*
         if (sound1) {
             auto soundPos = irrklang::vec3df(lightPos.x, lightPos.y, lightPos.z);
             sound1->setPosition(soundPos);
         }
         */

         // cleanup audio
            //if (sound1) sound1->drop(); // release music stream.
            //engine->drop();

namespace lux {

    class Sound
    {
    public:
    private:
    };
}
