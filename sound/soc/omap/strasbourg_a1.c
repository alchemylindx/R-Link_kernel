/*
 * strasbourg_a1.c  --  SoC audio for Strasbourg
 *
 * Author: Misael Lopez Cruz <x0052729@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <linux/clk.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <mach/gpio.h>

#include <plat/mcbsp.h>
#include <plat/mux.h>
#include <plat/omap-pm.h>
#include <plat/nashville.h>

#include "omap-mcbsp.h"
#include "omap-pcm.h"
#include "../codecs/twl4030.h"
#include "../codecs/bc6.h"

static struct snd_soc_dai_link strasbourg_a1_dai[];
static int strasbourg_a1_hifi_playback_state;
static int strasbourg_a1_voice_state;
static int strasbourg_a1_capture_state;

extern struct tt_scenario_ops twl4030_strasbourg_scenario_ops;

/* **************************** i2s configuration **************** */
static int strasbourg_a1_i2s_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	int ret;

	/* Set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai,
				  SND_SOC_DAIFMT_I2S |
				  SND_SOC_DAIFMT_NB_NF |
				  SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0) {
		printk(KERN_ERR "can't set codec DAI configuration\n");
		return ret;
	}

	/* Set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai,
				  SND_SOC_DAIFMT_I2S |
				  SND_SOC_DAIFMT_NB_NF |
				  SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0) {
		printk(KERN_ERR "can't set cpu DAI configuration\n");
		return ret;
	}

	/* Set the codec system clock for DAC and ADC */
	ret = snd_soc_dai_set_sysclk(codec_dai, 0, 26000000,
					SND_SOC_CLOCK_IN);
	if (ret < 0) {
		printk(KERN_ERR "can't set codec system clock\n");
		return ret;
	}

	/* Use external clock for mcBSP2 */
	ret = snd_soc_dai_set_sysclk(cpu_dai, OMAP_MCBSP_SYSCLK_CLKS_EXT,
			0, SND_SOC_CLOCK_OUT);

	return 0;
}

static int strasbourg_a1_i2s_hw_free(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	int ret;

	/* Use function clock for mcBSP2 */
	ret = snd_soc_dai_set_sysclk(cpu_dai, OMAP_MCBSP_SYSCLK_CLKS_FCLK,
			0, SND_SOC_CLOCK_OUT);
	return 0;
}

static int snd_hw_latency;
extern void omap_dpll3_errat_wa(int disable);
static int strasbourg_a1_i2s_startup(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;

	/*
	 * Hold C2 as min latency constraint. Deeper states
	 * MPU RET/OFF is overhead and consume more power than
	 * savings.
	 * snd_hw_latency check takes care of playback and capture
	 * usecase.
	 */
	if (!snd_hw_latency++) {
		omap_pm_set_max_mpu_wakeup_lat(rtd->socdev->dev, 18);
		/*
		 * As of now for MP3 playback case need to enable dpll3
		 * autoidle part of dpll3 lock errata.
		 * REVISIT: Remove this, Once the dpll3 lock errata is
		 * updated with with a new workaround without impacting mp3 usecase.
		 */
		omap_dpll3_errat_wa(0);
	}

	return 0;
}

static void strasbourg_a1_i2s_shutdown(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;

	/* remove latency constraint */
	snd_hw_latency--;
	if (!snd_hw_latency) {
		omap_pm_set_max_mpu_wakeup_lat(rtd->socdev->dev, -1);
		omap_dpll3_errat_wa(1);
	}
}

static struct snd_soc_ops strasbourg_a1_ops = {
	.hw_params = strasbourg_a1_i2s_hw_params,
	.hw_free = strasbourg_a1_i2s_hw_free,
	.startup = strasbourg_a1_i2s_startup,
	.shutdown = strasbourg_a1_i2s_shutdown,
};

/* **************************** Voice configuration **************** */
static int strasbourg_a1_voice_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	int ret;

	/* Set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai,
				SND_SOC_DAIFMT_DSP_A |
				SND_SOC_DAIFMT_IB_NF |
				SND_SOC_DAIFMT_CBS_CFM);

	if (ret) {
		printk(KERN_ERR "can't set codec DAI configuration\n");
			return ret;
	}

	/* set codec Voice IF to application mode*/
	ret = snd_soc_dai_set_tristate(codec_dai, 0);

	if (ret) {
			printk(KERN_ERR "can't disable codec VIF tristate\n");
		return ret;
	}

	/* Set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai,
				SND_SOC_DAIFMT_DSP_A |
				SND_SOC_DAIFMT_IB_NF |
				SND_SOC_DAIFMT_CBM_CFM);

	if (ret < 0) {
		printk(KERN_ERR "can't set cpu DAI configuration\n");
		return ret;
	}

	/* Set the codec system clock for DAC and ADC */
	ret = snd_soc_dai_set_sysclk(codec_dai, 0, 26000000,
					SND_SOC_CLOCK_IN);
	if (ret < 0) {
		printk(KERN_ERR "can't set codec system clock\n");
		return ret;
	}

	return 0;
}

int strasbourg_a1_voice_hw_free(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	int ret;

	/* set codec Voice IF to tristate*/
	ret = snd_soc_dai_set_tristate(codec_dai, 1);

	if (ret) {
			printk(KERN_ERR "can't set codec VIF tristate\n");
			return ret;
	}

	return 0;
}
static struct snd_soc_ops strasbourg_a1_voice_ops = {
	.hw_params = strasbourg_a1_voice_hw_params,
	.hw_free = strasbourg_a1_voice_hw_free,
};

/* ************************* BT PCM configuration ****************** */
static int strasbourg_a1_pcm_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	int ret;

	/* Set the cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_DSP_B
					 | SND_SOC_DAIFMT_IB_NF
					 | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0) {
		printk(KERN_ERR "can't set cpu DAI configuration\n");
		return ret;
	}

	/* Get CLKR from CLKX pin */
	ret = snd_soc_dai_set_sysclk(cpu_dai, OMAP_MCBSP_CLKR_SRC_CLKX, 0, SND_SOC_CLOCK_IN);
	if (ret < 0) {
		printk(KERN_ERR "can't set MCBSP1 CLKR mux setting\n");
		return ret;
	}

	/* Get FSR from FSX pin */
	ret = snd_soc_dai_set_sysclk(cpu_dai, OMAP_MCBSP_FSR_SRC_FSX, 0, SND_SOC_CLOCK_IN);
	if (ret < 0) {
		printk(KERN_ERR "can't set MCBSP1 FSR mux setting\n");
		return ret;
	}

	/* Use internal clock (CORE_96M_FCLK) for ops in SoC-Master mode */
	ret = snd_soc_dai_set_sysclk(cpu_dai, OMAP_MCBSP_SYSCLK_CLKS_FCLK,
				     96000000, SND_SOC_CLOCK_OUT);
	if (ret < 0) {
		printk(KERN_ERR "can't set cpu DAI clock configuration\n");
		return ret;
	}
	
	/* Set codec PCM division for sample rate */
	ret = snd_soc_dai_set_clkdiv(cpu_dai, OMAP_MCBSP_CLKGDV, 200);
	if (ret < 0) {
		printk(KERN_ERR "can't set cpu DAI divider configuration\n");
		return ret;
	}
	
	return 0;
}

static struct snd_soc_ops strasbourg_a1_pcm_ops = {
	.hw_params = strasbourg_a1_pcm_hw_params,
};

/* Audio Sampling frequences supported by Triton */
static const char *audio_sample_rates_txt[] = {
	"8000", "11025", "12000", "16000", "22050",
	"24000", "32000", "44100", "48000", "96000"
	};

/*
 * APLL_RATE defined in CODEC_MODE register, which corresponds
 * to the sampling rates defined above
 */
static const unsigned int audio_sample_rates_apll[] = {
	0x0, 0x1, 0x2, 0x4, 0x5,
	0x6, 0x8, 0x9, 0xa, 0xe
	};

/* Voice Sampling rates supported by Triton */
static const char *voice_sample_rates_txt[] = {
	"8000", "16000"
	};

/*
 * SEL_16K defined in CODEC_MODE register, which corresponds
 * to the voice sample rates defined above
 */
static const unsigned int voice_sample_rates_sel_16k[] = {
	0x0, 0x1
	};

static const struct soc_enum twl4030_audio_sample_rates_enum =
	SOC_VALUE_ENUM_SINGLE(TWL4030_REG_CODEC_MODE, 4, 0xf,
			ARRAY_SIZE(audio_sample_rates_txt),
			audio_sample_rates_txt, audio_sample_rates_apll);

static const struct soc_enum twl4030_voice_sample_rates_enum =
	SOC_VALUE_ENUM_SINGLE(TWL4030_REG_CODEC_MODE, 3, 0x1,
			ARRAY_SIZE(voice_sample_rates_txt),
			voice_sample_rates_txt, voice_sample_rates_sel_16k);

/* Strasbourg machine DAPM */
static const struct snd_soc_dapm_widget strasbourg_a1_twl4030_dapm_widgets[] = {
	SND_SOC_DAPM_MIC("Ext Mic", NULL),
	SND_SOC_DAPM_SPK("Ext Spk", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
	SND_SOC_DAPM_HP("Headset Stereophone", NULL),
	SND_SOC_DAPM_LINE("Aux In", NULL),
};

static const struct snd_soc_dapm_route audio_map[] = {
	/* External Mics: MAINMIC, SUBMIC with bias*/
	{"MAINMIC", NULL, "Mic Bias 1"},
	{"SUBMIC", NULL, "Mic Bias 2"},
	{"Mic Bias 1", NULL, "Ext Mic"},
	{"Mic Bias 2", NULL, "Ext Mic"},

	/* External Speakers: HFL, HFR */
	{"Ext Spk", NULL, "HFL"},
	{"Ext Spk", NULL, "HFR"},

	/* Headset Stereophone:  HSOL, HSOR */
	{"Headset Stereophone", NULL, "HSOL"},
	{"Headset Stereophone", NULL, "HSOR"},

	/* Headset Mic: HSMIC with bias */
	{"HSMIC", NULL, "Headset Mic Bias"},
	{"Headset Mic Bias", NULL, "Headset Mic"},

	/* Aux In: AUXL, AUXR */
	{"Aux In", NULL, "AUXL"},
	{"Aux In", NULL, "AUXR"},
};

static int strasbourg_a1_get_hifi_playback_state(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = strasbourg_a1_hifi_playback_state;
	return 0;
}

static int strasbourg_a1_set_hifi_playback_state(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	int ret;

	if (strasbourg_a1_hifi_playback_state == ucontrol->value.integer.value[0])
		return 0;

	if (ucontrol->value.integer.value[0]) {
		ret = snd_soc_dapm_stream_event(strasbourg_a1_dai[0].codec_dai->codec,
				strasbourg_a1_dai[0].codec_dai->playback.stream_name,
				SND_SOC_DAPM_STREAM_START);
	} else {
		ret = snd_soc_dapm_stream_event(strasbourg_a1_dai[0].codec_dai->codec,
				strasbourg_a1_dai[0].codec_dai->playback.stream_name,
				SND_SOC_DAPM_STREAM_STOP);
	}

	if (ret != 0) {
		printk(KERN_ERR "failed to set hifi playback state\n");
		return 0;
	}

	strasbourg_a1_hifi_playback_state = ucontrol->value.integer.value[0];
	return 1;
}

static int strasbourg_a1_get_voice_state(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = strasbourg_a1_voice_state;
	return 0;
}

static int strasbourg_a1_set_voice_state(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	int ret;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);

	if (strasbourg_a1_voice_state == ucontrol->value.integer.value[0])
		return 0;

	if (ucontrol->value.integer.value[0]) {
		ret = snd_soc_dapm_stream_event(strasbourg_a1_dai[1].codec_dai->codec,
				strasbourg_a1_dai[1].codec_dai->playback.stream_name,
				SND_SOC_DAPM_STREAM_START);

		/* Enable voice digital filters */
		snd_soc_update_bits(codec, TWL4030_REG_OPTION,
				    TWL4030_ARXL1_VRX_EN, 0x10);
	} else {
		ret = snd_soc_dapm_stream_event(strasbourg_a1_dai[1].codec_dai->codec,
				strasbourg_a1_dai[1].codec_dai->playback.stream_name,
				SND_SOC_DAPM_STREAM_STOP);

		/* Disable voice digital filters */
		snd_soc_update_bits(codec, TWL4030_REG_OPTION,
				    TWL4030_ARXL1_VRX_EN, 0x0);
	}

	if (ret != 0) {
		printk(KERN_ERR "failed to set voice playback state\n");
		return 0;
	}

	strasbourg_a1_voice_state = ucontrol->value.integer.value[0];
	return 1;
}

static int strasbourg_a1_get_capture_state(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = strasbourg_a1_capture_state;
	return 0;
}

static int strasbourg_a1_set_capture_state(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	int ret;

	if (strasbourg_a1_capture_state == ucontrol->value.integer.value[0])
		return 0;

	if (ucontrol->value.integer.value[0]) {
		ret = snd_soc_dapm_stream_event(strasbourg_a1_dai[1].codec_dai->codec,
				strasbourg_a1_dai[1].codec_dai->capture.stream_name,
				SND_SOC_DAPM_STREAM_START);
	} else {
		ret = snd_soc_dapm_stream_event(strasbourg_a1_dai[1].codec_dai->codec,
				strasbourg_a1_dai[1].codec_dai->capture.stream_name,
				SND_SOC_DAPM_STREAM_STOP);
	}

	if (ret != 0) {
		printk(KERN_ERR "failed to set capture state\n");
		return 0;
	}

	strasbourg_a1_capture_state = ucontrol->value.integer.value[0];
	return 1;
}

static const char *path_control[] = {"Off", "On"};

static const struct soc_enum strasbourg_a1_enum[] = {
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(path_control), path_control),
};

static const struct snd_kcontrol_new strasbourg_a1_controls[] = {
	SOC_ENUM_EXT("HIFI Playback Control", strasbourg_a1_enum[0],
		strasbourg_a1_get_hifi_playback_state, strasbourg_a1_set_hifi_playback_state),
	SOC_ENUM_EXT("Voice Control", strasbourg_a1_enum[0],
		strasbourg_a1_get_voice_state, strasbourg_a1_set_voice_state),
	SOC_ENUM_EXT("Capture Control", strasbourg_a1_enum[0],
		strasbourg_a1_get_capture_state, strasbourg_a1_set_capture_state),
	SOC_ENUM_EXT("Audio Sample Rate", twl4030_audio_sample_rates_enum,
		snd_soc_get_value_enum_double, snd_soc_put_value_enum_double),
	SOC_ENUM_EXT("Voice Sample Rate", twl4030_voice_sample_rates_enum,
		snd_soc_get_value_enum_double, snd_soc_put_value_enum_double),
	SOC_SINGLE("256FS CLK Control Switch", TWL4030_REG_AUDIO_IF, 1, 1, 0),
};

static int strasbourg_a1_twl4030_init(struct snd_soc_codec *codec)
{
	int ret;

	/* Add Strasbourg specific controls */
	ret = snd_soc_add_controls(codec, strasbourg_a1_controls,
				ARRAY_SIZE(strasbourg_a1_controls));
	if (ret)
		return ret;

	/* Add Strasbourg specific widgets */
	ret = snd_soc_dapm_new_controls(codec, strasbourg_a1_twl4030_dapm_widgets,
				ARRAY_SIZE(strasbourg_a1_twl4030_dapm_widgets));
	if (ret)
		return ret;

	/* Set up Strasbourg specific audio path audio_map */
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

#ifdef CONFIG_TOMTOM_NASHVILLE_SCENARI_TWL4030
	/* Add nashville controls */
	tomtom_add_nashville_controls(codec, &twl4030_strasbourg_scenario_ops);
#endif

	/* Strasbourg connected pins */
	snd_soc_dapm_enable_pin(codec, "Ext Mic");
	snd_soc_dapm_enable_pin(codec, "Ext Spk");
	snd_soc_dapm_enable_pin(codec, "Headset Mic");
	snd_soc_dapm_enable_pin(codec, "Headset Stereophone");
	snd_soc_dapm_enable_pin(codec, "Aux In");

	/* TWL4030 not connected pins */
	snd_soc_dapm_nc_pin(codec, "CARKITMIC");
	snd_soc_dapm_nc_pin(codec, "DIGIMIC0");
	snd_soc_dapm_nc_pin(codec, "DIGIMIC1");

	snd_soc_dapm_nc_pin(codec, "OUTL");
	snd_soc_dapm_nc_pin(codec, "OUTR");
	snd_soc_dapm_nc_pin(codec, "EARPIECE");
	snd_soc_dapm_nc_pin(codec, "PREDRIVEL");
	snd_soc_dapm_nc_pin(codec, "PREDRIVER");
	snd_soc_dapm_nc_pin(codec, "CARKITL");
	snd_soc_dapm_nc_pin(codec, "CARKITR");

	ret = snd_soc_dapm_sync(codec);

	return ret;
}

static int strasbourg_a1_twl4030_voice_init(struct snd_soc_codec *codec)
{
	unsigned short reg;

	/* Enable voice interface */
	reg = codec->read(codec, TWL4030_REG_VOICE_IF);
	reg |= TWL4030_VIF_DIN_EN | TWL4030_VIF_DOUT_EN | TWL4030_VIF_EN;
	codec->write(codec, TWL4030_REG_VOICE_IF, reg);

	return 0;
}

static int strasbourg_a1_bc6_init(struct snd_soc_codec *codec)
{
	return 0;
}

/* Digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link strasbourg_a1_dai[] = {
	{
		.name = "TWL4030 I2S",
		.stream_name = "TWL4030 Audio",
		.cpu_dai = &omap_mcbsp_dai[0],
		.codec_dai = &twl4030_dai[TWL4030_DAI_HIFI],
		.init = strasbourg_a1_twl4030_init,
		.ops = &strasbourg_a1_ops,
	},
	{
		.name = "TWL4030 PCM",
		.stream_name = "TWL4030 Voice",
		.cpu_dai = &omap_mcbsp_dai[1],
		.codec_dai = &twl4030_dai[TWL4030_DAI_VOICE],
		.init = strasbourg_a1_twl4030_voice_init,
		.ops = &strasbourg_a1_voice_ops,
	},
	{
		.name = "Bluetooth PCM",
		.stream_name = "Bluetooth PCM",
		.cpu_dai = &omap_mcbsp_dai[2],
		.codec_dai = &bc6_dai,
		.init = strasbourg_a1_bc6_init,
		.ops = &strasbourg_a1_pcm_ops,
	},

};

/* Audio machine driver */
static struct snd_soc_card snd_soc_strasbourg_a1 = {
	.name = "Strasbourg A1",
	.platform = &omap_soc_platform,
	.dai_link = strasbourg_a1_dai,
	.num_links = ARRAY_SIZE(strasbourg_a1_dai),
};

/* EXTMUTE callback function */
void strasbourg_a1_set_hs_extmute(int mute)
{
}

/* twl4030 setup */
static struct twl4030_setup_data twl4030_setup = {
	.ramp_delay_value = 3,	/* 161 ms */
	.sysclk = 26000,
	.hs_extmute = 1,
	.set_hs_extmute = strasbourg_a1_set_hs_extmute,
};

/* Audio subsystem */
static struct snd_soc_device strasbourg_a1_snd_devdata = {
	.card = &snd_soc_strasbourg_a1,
	.codec_dev = &soc_codec_dev_twl4030,
	.codec_data = &twl4030_setup,
};

static struct platform_device *strasbourg_a1_snd_device;

static int __init strasbourg_a1_soc_init(void)
{
	int ret;

	if (!machine_is_strasbourg()) {
		pr_debug("Not Strasbourg!\n");
		return -ENODEV;
	}
	pr_info("Strasbourg SoC init\n");

	strasbourg_a1_snd_device = platform_device_alloc("soc-audio", -1);
	if (!strasbourg_a1_snd_device) {
		printk(KERN_ERR "Platform device allocation failed\n");
		return -ENOMEM;
	}

	platform_set_drvdata(strasbourg_a1_snd_device, &strasbourg_a1_snd_devdata);
	strasbourg_a1_snd_devdata.dev = &strasbourg_a1_snd_device->dev;
	*(unsigned int *)strasbourg_a1_dai[0].cpu_dai->private_data = 1; /* McBSP2 */
	*(unsigned int *)strasbourg_a1_dai[1].cpu_dai->private_data = 3; /* McBSP4 */
	*(unsigned int *)strasbourg_a1_dai[2].cpu_dai->private_data = 0; /* McBSP1 */

	ret = platform_device_add(strasbourg_a1_snd_device);
	if (ret)
		goto err1;

	return 0;

err1:
	printk(KERN_ERR "Unable to add platform device\n");
	platform_device_put(strasbourg_a1_snd_device);

	return ret;
}
module_init(strasbourg_a1_soc_init);

static void __exit strasbourg_a1_soc_exit(void)
{
	platform_device_unregister(strasbourg_a1_snd_device);
}
module_exit(strasbourg_a1_soc_exit);

MODULE_AUTHOR("Misael Lopez Cruz <x0052729@ti.com>");
MODULE_AUTHOR("Guillaume Ballet <guillaume.ballet@tomtom.com>");
MODULE_DESCRIPTION("ALSA SoC Strasbourg A1");
MODULE_LICENSE("GPL");

