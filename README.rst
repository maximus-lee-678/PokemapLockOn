.. |everpresence image| image:: https://github.com/maximus-lee-678/PokemapLockOn/blob/main/PokemapLockOn/resource/favicon.png
  :width: 24px
  :alt: PokemapLockOn Logo

|everpresence image| PokemapLockOn
=================

| C Windows application that polls `sgpokemap.com <https://sgpokemap.com/gym.html>`_ to obtain raid details at set intervals.
| Get the latest release `here <https://github.com/maximus-lee-678/PokemapLockOn/releases>`_.
| ℹ Anti-viruses may flag this executable as a virus, which may cause it to not run properly or be deleted.
|
| #HearUsNiantic

📃 Features
------------

1. Modifiable gym list
2. Find gym by name search or proximity from specified coordinates
3. Configurable polling time interval
4. Choose which raid tiers to display and notify
5. Light memory footprint

image:: https://github.com/maximus-lee-678/PokemapLockOn/blob/main/PokemapLockOn/resource/sample_options.png
  :width: 256px
  :alt: PokemapLockOn Options Menu

🚓 Roadmap
-----------

* Currently does not properly support gyms with UTF-8 encoding, retrieves the gyms but displays gibberish.

🖥️ VS2022 Setup
---------------

This program was developed in VS2022 using libcurl for Windows.
libcurl was installed using `vcpkg <https://github.com/microsoft/vcpkg>`_.

1. Get `latest <https://github.com/microsoft/vcpkg/releases>`_ vcpkg zip file and extract it.
2. Open **Developer Command Prompt for VS** and cd to wherever vckpg has been extracted to.
3. Run:

.. code-block:: console

  bootstrap-vcpkg.bat

4. Run:

.. code-block:: console

  vcpkg.exe integrate install

5. Run:

.. code-block:: console

  vcpkg.exe install curl

OR

.. code-block:: console

  vcpkg.exe install curl --triplet=x64

6. After installation, in the project: ensure solution configuration (next to debug buttons) is targeting right triplet (x86 or x64).
