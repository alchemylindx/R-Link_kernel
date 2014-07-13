/*
 * s5m8752-i2c.c  --  S5M8752 Advanced PMIC with AUDIO Codec
 *
 * Copyright 2010 Samsung Electronics.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#include <linux/i2c.h>
#include <linux/mfd/s5m8752/core.h>
#include <linux/slab.h>

/******************************************************************************
 * This function reads the i2c bus by one byte.
 ******************************************************************************/
static int s5m8752_i2c_read_device(struct s5m8752 *s5m8752, uint8_t reg,
					uint8_t *val)
{
	int ret;
	ret = i2c_smbus_read_byte_data(s5m8752->i2c_client, reg);
	if (ret < 0) {
		dev_err(s5m8752->dev, "failed reading at 0x%02x\n", reg);
		return ret;
	}
	*val = (uint8_t)ret;
	return 0;
}

/******************************************************************************
 * This function writes the i2c bus by one byte.
 ******************************************************************************/
static int s5m8752_i2c_write_device(struct s5m8752 *s5m8752, uint8_t reg,
					uint8_t val)
{
	int ret;
	ret = i2c_smbus_write_byte_data(s5m8752->i2c_client, reg, val);
	if (ret < 0) {
		dev_err(s5m8752->dev, "failed writing 0x%02x to 0x%02x\n",
				val, reg);
		return ret;
	}
	return 0;
}

/******************************************************************************
 * This function reads the i2c bus by several bytes.
 ******************************************************************************/
static int s5m8752_i2c_read_block_device(struct s5m8752 *s5m8752, uint8_t reg,
					int len, uint8_t *val)
{
	int ret;
	ret = i2c_smbus_read_i2c_block_data(s5m8752->i2c_client, reg, len, val);
	if (ret < 0) {
		dev_err(s5m8752->dev, "failed reading from 0x%02x\n", reg);
		return ret;
	}
	return 0;
}

/******************************************************************************
 * This function writes the i2c bus by several bytes.
 ******************************************************************************/
static int s5m8752_i2c_write_block_device(struct s5m8752 *s5m8752, uint8_t reg,
					int len, uint8_t *val)
{
	int ret;
	ret = i2c_smbus_write_i2c_block_data(s5m8752->i2c_client, reg, len,
									val);
	if (ret < 0) {
		dev_err(s5m8752->dev, "failed writings to 0x%02x\n", reg);
		return ret;
	}
	return 0;
}

/******************************************************************************
 * This function makes sure that a S5M8752 is attached to the I2C bus
 * and calls the S5M8752 device init function.
 ******************************************************************************/
static int s5m8752_probe(struct i2c_client *i2c,
					const struct i2c_device_id *id)
{
	struct s5m8752 *s5m8752;

	s5m8752 = kzalloc(sizeof(struct s5m8752), GFP_KERNEL);
	if (s5m8752 == NULL) {
		kfree(i2c);
		return -ENOMEM;
	}

	i2c_set_clientdata(i2c, s5m8752);
	s5m8752->dev = &i2c->dev;
	s5m8752->i2c_client = i2c;
	s5m8752->control_data = i2c;
	s5m8752->read_dev = s5m8752_i2c_read_device;
	s5m8752->write_dev = s5m8752_i2c_write_device;
	s5m8752->read_block_dev = s5m8752_i2c_read_block_device;
	s5m8752->write_block_dev = s5m8752_i2c_write_block_device;

	return s5m8752_device_init(s5m8752, i2c->irq, i2c->dev.platform_data);
}

static int s5m8752_remove(struct i2c_client *i2c)
{
	struct s5m8752 *s5m8752 = i2c_get_clientdata(i2c);

	s5m8752_device_exit(s5m8752);

	return 0;
}

static const struct i2c_device_id s5m8752_i2c_id[] = {
	{ "s5m8752", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, s5m8752_i2c_id);

static struct i2c_driver s5m8752_driver = {
	.driver	= {
		.name	= "s5m8752",
		.owner	= THIS_MODULE,
	},
	.probe		= s5m8752_probe,
	.remove		= s5m8752_remove,
	.id_table	= s5m8752_i2c_id,
};

static int __init s5m8752_init(void)
{
	return i2c_add_driver(&s5m8752_driver);
}
module_init(s5m8752_init);

static void __exit s5m8752_exit(void)
{
	i2c_del_driver(&s5m8752_driver);
}
module_exit(s5m8752_exit);

MODULE_DESCRIPTION("S5M752 Power Management IC");
MODULE_AUTHOR("Jongbin, Won <jongbin.won@samsung.com>");
MODULE_LICENSE("GPL");
