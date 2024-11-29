#!/bin/bash

uploadCode(){

	make upload
	pio device monitor

}

uploadCode
