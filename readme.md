# Example Multi project for MPLAB Discover Portal deployment

This example shows how to upload GitHub repository on MPLAB Discover portal. This example uses portal-client-launcher tool to upload GitHub repository on MPLAB Discover portal.

## Procedure: Setup

This document uses jargon vocabulary. For details, please refer to: [Integrating with Jenkins - Theory](https://confluence.microchip.com/display/SOLUTIONS/Integration+with+Jenkins+Remote+-+Theory+and+Prerequisites#IntegrationwithJenkinsRemote-TheoryandPrerequisites-theory)

### Prerequisites:
 
 1. [Jenkins and Bitbucket Project Linkage and Configuration](https://confluence.microchip.com/display/SOLUTIONS/Integration+with+Jenkins+Remote+-+Theory+and+Prerequisites#IntegrationwithJenkinsRemote-TheoryandPrerequisites-linkage) - Only needed if you want to setup the Entire Bitbucket Project with Jenkins in one setup. 
 2. [Automated Build trigger](https://confluence.microchip.com/display/SOLUTIONS/Integration+with+Jenkins+Remote+-+Theory+and+Prerequisites#IntegrationwithJenkinsRemote-TheoryandPrerequisites-trigger)
 3. [Create or Request Docker Images](https://confluence.microchip.com/display/SOLUTIONS/Integration+with+Jenkins+Remote+-+Theory+and+Prerequisites#IntegrationwithJenkinsRemote-TheoryandPrerequisites-docker) - Required for Build and Test Environment. 
 
### Setup Remote Repository 

1. Setup Build and Test Environment using Docker
    - Copy the cloudprovider.yml file from the existing example repository into your bitbucket repository. _Make sure you place the .yml file at the root of your repository (outside the .X/any folders)_
    - Update the clouldprovider.yml file to point to the correct docker image location by changing the `image` attribute in the file. _You will get the location of the Docker image for the tool and the desired version in the [artifactory](https://artifacts.microchip.com:7999/artifactory/docker/microchip/citd/)_.

        ![Example of yml](images/yml.PNG?raw=true "Example of yml")

2. Setup metadata
    The metadata file must be required to upload GitHub repository. If there is no metadata file in your bitbucket repository, create `.main-meta` folder at the root of your repository and project folders, copy and paste `main.json` file from the existing example repository. The example `main.json` file is available [here](.main-meta/main.json).
    - User should update following project properties in the `main.json` as per the project. 

        ![Example of File](images/main.PNG?raw=true "Example of File")

3. Setup Jenkins Stage(s) 

 -   If there is no Jenkinsfile in your bitbucket repository, following steps are required for the Jenkins build pipeline setup.
        -  Copy the Jenkinsfilek8s from the existing example's repository into your bitbucket repository. Make sure you place the Jenkinsfilek8s at the root of your repository (outside the .X/any folders).
        - Update the label under kubernetes to reflect your project name (.X, java etc.). Updating the label with your project name is just a recommendation. It can consume any arbitrary value. 
        - You may configure your default notification reviewers by amending the NOTIFICATION_EMAIL parameter for the success and failure conditions.

            ![Example of kubernetes](images/kubernetes.PNG?raw=true "Example of kubernetes")   

-  Configure the following environment variables as per your project requirement.
    - BITBUCKET_SOURCE_URL :- BitBucket source repo URL to be deployed
    - GITHUB_CREDENTIAL_ID :- GitHub Personal access token stored from jenkins credential store
    - GITHUB_URL :- GitHub project repository URL.
    - DEPLOY_EXCLUDE_FOLDER_FILE_LIST :- Files or folders to be excluded from deployment, if multiple files or folders use comma separator.
    - DEPLOY_BRANCH_LIST :- GitHub Deploy branch list
    - CHANGE_LOG_PATH :-  GitHub Release creation changelog file path
    - SOURCE_PROJECT_META_DATA: Meta data file path
    - DEPLOY_TOOL_URL :- GitHub deploy tool URL
    ![Example of env](images/env.PNG?raw=true "Example of env")  
- Following are the Jenkins stages for this example.
    - **Checkout**:- This stage is a mandatory for Jenkins pipeline. This stage checkout the bitbucket project repository.
    - **Build**:- This stage may be optional for your bitbucket project. This stage builds the MPLABX IDE project (.X ) using tool-mplabx-c-build.
        - mplabx-c-build tool
            - Change the Source Projects Folder Path (sp) and the Report generation folder path (rp) arguments based on your project structure.
            - For details on the arguments, look here: https://bitbucket.microchip.com/projects/CITD/repos/tool-mplabx-c-build/browse

            ![Example of build](images/build.PNG?raw=true "Example of build")  
    - **GitHub tool clone**:- This stage clone the GitHub deployment tool
             ![Example of github](images/github_tool.PNG?raw=true "Example of github")
    - **GitHub Repo Creation**:- This stage create GitHub repository using meta data file inputs(.meta-data/main.json). User can modified this stage as per the project requirement.
            - github-deploy tool
                - For more details on github-deploy tool, look here: https://bitbucket.microchip.com/projects/CITD/repos/tool-github-deploy/browse

           ![Example of github](images/github_create.PNG?raw=true "Example of github")
    - **GitHub Deploy Source**:- This stage deploys bitbucket repository in GitHub repository using tool-gihub-deploy. This stage deploys the bitbucket repository as it is on GitHub repository. User can modified this stage as per the project requirement.
        - github-deploy tool
            - For more details on github-deploy tool, look here: https://bitbucket.microchip.com/projects/CITD/repos/tool-github-deploy/browse

             ![Example of github](images/github_deploy.PNG?raw=true "Example of github")
  - **GitHub release**:- This stage create GitHub repository release tag in GitHub repository using tool-gihub-deploy. This stage deploys the bitbucket repository as it is on GitHub repository. User can modified this stage as per the project requirement.
        - github-release creation tool
            - For more details on github-deploy tool, look here: https://bitbucket.microchip.com/projects/CITD/repos/tool-github-deploy/browse

  - **Portal-Deploy**:- This stage uploads GitHub project repository on MPLAB Discover portal. 
    - Portal deploy categorized two category  
        - Single project deployment
        - multi project deployment    
    -   This stage reads `repository name` and `version` from the metadata file. It also  reads the value of environment variable `GITHUB_OWNER`. These parameters must be required for portal deployment.
    -   When version of metadata file matches with release tag, this stage runs portal-client-launcher tool to upload GitHub project repository.
    - portal-client-launcher tool
        - For more details on portal-client-launcher tool, look here: https://bitbucket.microchip.com/projects/CITD/repos/tool-portal-client-launcher/browse

        ![Example of portal](images/portal-1.PNG?raw=true "Example of portal")
        ![Example of portal](images/portal-2.PNG?raw=true "Example of portal")

 **For more details refer**: [Jenkinsfilek8s](Jenkinsfilek8s)\
 **For more details refer**: [Confluence: Jenkins Project](https://confluence.microchip.com/display/MSDTC/Corporate+DevOps+Jenkins)

 
