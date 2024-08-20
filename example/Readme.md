# Guide

## Overview

This document illustrates how to create and run a Docker image to reproduce a example for ReachCheck.
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
   docker build --pull --rm -f "./DC/DockerFile" -t reachcheck_dc:latest "./DC"
   docker build --pull --rm -f "./CVE/DockerFile" -t reachcheck_cve:latest "./CVE"
   ```

## Step 2: Run Docker Image

Start a container instance with the following command:

   ```bash
   docker run reachcheck_dc:latest
   docker run reachcheck_cve:latest
   ```