# SVD files to use in launch.json
- [Reference from Steelph0enix](https://steelph0enix.github.io/posts/vscode-cubemx-setup/#configuring-the-peripheral-view)


1. Go to controller web site, e.g. for [STM32L476RG](https://www.st.com/en/microcontrollers-microprocessors/stm32l476rg.html)
2. Under CAD-Resources select `System View Description` and download .zip archive
3. Extract the archive. It should contain an `*.svd` file.
4. Copy this corresponding file into this folder.
5. In launch.json, add
  ```json
  "svdFile": "/workspaces/stm32/svds/STM32L4x6.svd"
  ```