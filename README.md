# linux-loader
**An ARM firmware image for the Wii U, designed to work with [linux-wiiu/linux-wiiu](https://gitlab.com/linux-wiiu/linux-wiiu).**

### Getting linux-loader
##### Prebuilt Download
A precompiled fw.img is available [here](https://gitlab.com/linux-wiiu/linux-loader/-/jobs/artifacts/master/raw/fw.img?job=loader-build). Don't think about it too much. Once you've got that, jump down to "Setup" and keep reading.

##### Compiling (Docker)
The preferred method of compiling is using our Docker image. Clone or download this repository, then:
```sh
#replace with wherever you cloned this repo
cd path/to/linux-loader
#Replace with the Starbuck key and IV, respectively. Make sure it's in all caps
sed s/B5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX/STARBUCK_KEY_HERE/g -i castify.py
sed s/91XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX/STARBUCK_IV_HERE/g -i castify.py
#Run the image! (also works in Podman)
docker run --rm -it -v $(pwd):/app registry.gitlab.com/linux-wiiu/linux-loader/ci
```
You should now find a fw.img in your linux-loader folder! Congrats. Read the "Setup" section next!

##### Compiling (from scratch, not recommended)
First, edit `castify.py` to add the Starbuck key/IV - this is required to work with current CFW loaders. Then, just run make. You'll need devkitARM installed. You should end up with a fw.img, along with a bunch of ELF files.

### Setup
You'll need an SD card normally readable by the Wii U. Simply copy the fw.img to the root of the SD; then copy `dtbImage.wiiu` to `sd:/linux` (you should end up with `sd:/linux/dtbImage.wiiu`).

### Running
Once you've done everything in the Setup stage, just run your favorite CFW booter (CBHC works, as does the old wiiubru loader, and the Aroma beta). With any luck, linux-loader will start and boot up the Linux kernel.

### Advanced Setup (boot.cfg)
If you'd like to boot a different kernel or customise your commandline (to change the rootfs, for example) you can make a configuration file at `sd:/linux/boot.cfg`:
```ini
[loader]
default=debian

[profile:debian]
name=Debian SID
kernel=sdmc:/linux/dtbImage.wiiu
cmdline=root=/dev/mmcblk0p2 rootwait
```
linux-loader's boot.cfg uses the concept of "profiles" to set up configurations for the Linux kernel. Note the section `[profile:debian]` - this creates a profile named `debian` with all the settings that follow.

The first section, `[loader]`, defines settings to control linux-loader's behaviour. The only option it has right now is `default`, to set the profile to be booted. You can see in the example that this is set to `debian`, so linux-loader will find and use the settings under `[profile:debian]`. This option must be present to use the boot.cfg features. It's intended for forwards-compatibility in case we ever add a boot menu.

The example then contains the section `[profile:debian]`, which defines a profile named `debian`. In this section are the `name`, `kernel` and `cmdline` options. You must define at least one profile in a boot.cfg. The `name` option sets a human-readable name or nickname for your profile. The next option, `kernel`, tells linux-loader where to load dtbImage.wiiu from, and can be any path. **Please note the use of `sdmc:/` instead of `sd:/`!** If this option is not present or the path cannot be read, linux-loader will fall back on `sdmc:/linux/dtbImage.wiiu` then `sdmc:/linux/kernel`. The final option, `cmdline`, sets the kernel commandline to any string. This is useful to control the behaviour of the kernel - in the example above, we set the rootfs to `/dev/mmcblk0p2` (the SD card [`mmcblk0`] partition 2 [`p2`]) allowing us to boot from the SD card! None of these options are strictly needed to use a boot.cfg - linux-loader has defaults it will fall back on, though it's recommended to at least have `name` and `cmdline`.
