/*
 * arch/arm/mach-msm/kcal_ctrl.c
 *
 * Copyright (C) 2014 Savoca
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <mach/kcal.h>

#ifdef CONFIG_LCD_KCAL

static struct kcal_platform_data *kcal_pdata;
static int last_status_kcal_ctrl;

static ssize_t kcal_store(struct device *dev, struct device_attribute *attr,
						const char *buf, size_t count)
{
	int kcal_r = 0;
	int kcal_g = 0;
	int kcal_b = 0;

	if (!count)
		return -EINVAL;

	sscanf(buf, "%d %d %d", &kcal_r, &kcal_g, &kcal_b);
	kcal_pdata->set_values(kcal_r, kcal_g, kcal_b);
	return count;
}

static ssize_t kcal_show(struct device *dev, struct device_attribute *attr,
								char *buf)
{
	int kcal_r = 0;
	int kcal_g = 0;
	int kcal_b = 0;

	kcal_pdata->get_values(&kcal_r, &kcal_g, &kcal_b);

	return sprintf(buf, "%d %d %d\n", kcal_r, kcal_g, kcal_b);
}

static ssize_t kcal_ctrl_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int cmd = 0;

	if (!count)
		return last_status_kcal_ctrl = -EINVAL;

	sscanf(buf, "%d", &cmd);

	if(cmd != 1)
		return last_status_kcal_ctrl = -EINVAL;

	last_status_kcal_ctrl = kcal_pdata->refresh_display();

	if(last_status_kcal_ctrl)
		return -EINVAL;
	else
		return count;
}

static ssize_t kcal_ctrl_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	if(last_status_kcal_ctrl)
		return sprintf(buf, "NG\n");
	else
		return sprintf(buf, "OK\n");
}

static DEVICE_ATTR(kcal, 0644, kcal_show, kcal_store);
static DEVICE_ATTR(kcal_ctrl, 0644, kcal_ctrl_show, kcal_ctrl_store);

static ssize_t rgb_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int r, g, b;

	kcal_pdata->get_values(&r, &g, &b);

	sprintf(buf,   "last: %s\n", last_status_kcal_ctrl ? "NG" : "OK");
	sprintf(buf, "%srgb:  %d %d %d\n\n", buf, r, g, b);
	sprintf(buf, "%s::: COMMAND: update :::\n", buf);

	return strlen(buf);
}

static ssize_t rgb_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int r, g, b;

	if (sysfs_streq(buf, "update")) {
		last_status_kcal_ctrl = kcal_pdata->refresh_display();

		if(last_status_kcal_ctrl)
			return -EINVAL;
		else
			return count;
	}

	if (sscanf(buf, "%d %d %d", &r, &g, &b) == 3) {
		kcal_pdata->set_values(r, g, b);

		return count;
	}

	return -EINVAL;
}
static struct kobj_attribute rgb_interface = __ATTR(rgb, 0644, rgb_show, rgb_store);

static struct attribute *kcal_attrs[] = {
	&rgb_interface.attr, 
	NULL,
};

static struct attribute_group kcal_interface_group = {
	.attrs = kcal_attrs,
};

static struct kobject *kcal_kobject;

static int kcal_ctrl_probe(struct platform_device *pdev)
{
	int rc = 0;

	kcal_pdata = pdev->dev.platform_data;

	if(!kcal_pdata->set_values || !kcal_pdata->get_values ||
					!kcal_pdata->refresh_display) {
		return -1;
	}

	rc = device_create_file(&pdev->dev, &dev_attr_kcal);

	if(rc)
		return -1;

	rc = device_create_file(&pdev->dev, &dev_attr_kcal_ctrl);

	if(rc)
		return -1;

	kcal_kobject = kobject_create_and_add("kcal_color", kernel_kobj);

	if (!kcal_kobject) {
		pr_err("kcal: Failed to create kobject interface\n");
	}

	rc = sysfs_create_group(kcal_kobject, &kcal_interface_group);

	if (rc) {
		kobject_put(kcal_kobject);
	}

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = kcal_ctrl_probe,
	.driver = {
		.name   = "kcal_ctrl",
	},
};

int __init kcal_ctrl_init(void)
{
	return platform_driver_register(&this_driver);
}

device_initcall(kcal_ctrl_init);
#endif

MODULE_DESCRIPTION("LCD KCAL Driver");
MODULE_LICENSE("GPL v2");
