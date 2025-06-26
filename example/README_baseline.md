# Guide

## Overview

This document illustrates how to create and run a Docker image to reproduce a example for baseline.
Follow these steps to replicate the environment.

## Prerequisites

- Docker: Ensure you have the latest version of Docker installed.

## Step 1: Create Docker Image

1. **Navigate to the Example Directory**

   Change to the `example` directory where the example is located:

   ```bash
   cd ./example
   ```

2. **Build the Docker Image**

   In the example directory, build the image using the Dockerfile:

   ```bash
   docker build --pull --rm -f "./CVE_baseline/DockerFile" -t baseline_cve:latest "./CVE_baseline"
   docker build --pull --rm -f "./DC_baseline/DockerFile" -t baseline_dc:latest "./DC_baseline"
   ```

## Step 2: Run Docker Image

Start a container instance with the following command:

   ```bash
   docker run baseline_cve:latest
   docker run baseline_dc:latest
   ```
