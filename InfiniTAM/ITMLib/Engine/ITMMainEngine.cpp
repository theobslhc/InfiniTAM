// Copyright 2014-2015 Isis Innovation Limited and the authors of InfiniTAM

#include "ITMMainEngine.h"

using namespace ITMLib::Engine;

ITMMainEngine::ITMMainEngine(const ITMLibSettings *settings, const ITMRGBDCalib *calib, Vector2i imgSize_rgb, Vector2i imgSize_d)
{
	// create all the things required for marching cubes and mesh extraction
	// - uses additional memory (lots!)
	static const bool createMeshingEngine = true;

	if ((imgSize_d.x == -1) || (imgSize_d.y == -1)) imgSize_d = imgSize_rgb;

	this->settings = settings;

	this->scene = new ITMScene<ITMVoxel, ITMVoxelIndex>(&(settings->sceneParams), settings->useSwapping,
		settings->deviceType == ITMLibSettings::DEVICE_CUDA ? MEMORYDEVICE_CUDA : MEMORYDEVICE_CPU);

	meshingEngine = NULL;
	switch (settings->deviceType)
	{
	case ITMLibSettings::DEVICE_CPU:
		lowLevelEngine = new ITMLowLevelEngine_CPU();
		viewBuilder = new ITMViewBuilder_CPU(calib);
		visualisationEngine = new ITMVisualisationEngine_CPU<ITMVoxel, ITMVoxelIndex>(scene);
		if (createMeshingEngine) meshingEngine = new ITMMeshingEngine_CPU<ITMVoxel, ITMVoxelIndex>();
		break;
	case ITMLibSettings::DEVICE_CUDA:
#ifndef COMPILE_WITHOUT_CUDA
		lowLevelEngine = new ITMLowLevelEngine_CUDA();
		viewBuilder = new ITMViewBuilder_CUDA(calib);
		visualisationEngine = new ITMVisualisationEngine_CUDA<ITMVoxel, ITMVoxelIndex>(scene);
		if (createMeshingEngine) meshingEngine = new ITMMeshingEngine_CUDA<ITMVoxel, ITMVoxelIndex>();
#endif
		break;
	case ITMLibSettings::DEVICE_METAL:
#ifdef COMPILE_WITH_METAL
		lowLevelEngine = new ITMLowLevelEngine_Metal();
		viewBuilder = new ITMViewBuilder_Metal(calib);
		visualisationEngine = new ITMVisualisationEngine_Metal<ITMVoxel, ITMVoxelIndex>(scene);
		if (createMeshingEngine) meshingEngine = new ITMMeshingEngine_CPU<ITMVoxel, ITMVoxelIndex>();
#endif
		break;
	}

	/***** settings to string ****/
	/*
	std::cout << "use swappingggg " << settings->useSwapping << "\n";
	std::cout << "useApproximateRaycast " << settings->useApproximateRaycast << "\n";
	std::cout << "useBilateralFilter " << settings->useBilateralFilter << "\n";
	std::cout << "modelSensorNoise " << settings->modelSensorNoise << "\n";
	std::cout << "trackertype " << settings->trackerType << "\n";
	std::cout << "noHierarchyLevels " << settings->noHierarchyLevels << "\n";
	std::cout << "noICPRunTillLevel " << settings->noICPRunTillLevel << "\n";
	std::cout << "skipPoints " << settings->skipPoints << "\n";
	std::cout << "depthTrackerICPThreshold " << settings->depthTrackerICPThreshold << "\n";
	std::cout << "depthTrackerTerminationThreshold " << settings->depthTrackerTerminationThreshold << "\n";

	std::cout << "voxelSize " << settings->sceneParams.voxelSize << "\n";
	std::cout << "viewFrustum_min " << settings->sceneParams.viewFrustum_min << "\n";
	std::cout << "viewFrustum_max " << settings->sceneParams.viewFrustum_max << "\n";

	std::cout << "intrinsics_rgb fx " << calib->intrinsics_rgb.projectionParamsSimple.fx << "\n";
	std::cout << "intrinsics_rgb fy " << calib->intrinsics_rgb.projectionParamsSimple.fy << "\n";
	std::cout << "intrinsics_rgb cx " << calib->intrinsics_rgb.projectionParamsSimple.px << "\n";
	std::cout << "intrinsics_rgb cy " << calib->intrinsics_rgb.projectionParamsSimple.py << "\n";
	std::cout << "intrinsics_d fx " << calib->intrinsics_d.projectionParamsSimple.fx << "\n";
	std::cout << "intrinsics_d fy " << calib->intrinsics_d.projectionParamsSimple.fy << "\n";
	std::cout << "intrinsics_d cx " << calib->intrinsics_d.projectionParamsSimple.px << "\n";
	std::cout << "intrinsics_d cy " << calib->intrinsics_d.projectionParamsSimple.py << "\n";

	std::cout << "img size rgb " << imgSize_rgb.x << " x "<< imgSize_rgb.y << "\n";
	std::cout << "img size depth " << imgSize_d.x << " x "<< imgSize_d.y << "\n";

	std::cout << "Trafo RGB to depth " << calib->trafo_rgb_to_depth.calib.getValues()[0] << " " << calib->trafo_rgb_to_depth.calib.getValues()[1] << " " << calib->trafo_rgb_to_depth.calib.getValues()[2] << " " << calib->trafo_rgb_to_depth.calib.getValues()[3]<< "\n";
	std::cout << "                   " << calib->trafo_rgb_to_depth.calib.getValues()[4] << " " << calib->trafo_rgb_to_depth.calib.getValues()[5] << " " << calib->trafo_rgb_to_depth.calib.getValues()[6] << " " << calib->trafo_rgb_to_depth.calib.getValues()[7]<< "\n";
	std::cout << "                   " << calib->trafo_rgb_to_depth.calib.getValues()[8] << " " << calib->trafo_rgb_to_depth.calib.getValues()[9] << " " << calib->trafo_rgb_to_depth.calib.getValues()[10] << " " << calib->trafo_rgb_to_depth.calib.getValues()[11]<< "\n";
	std::cout << "                   " << calib->trafo_rgb_to_depth.calib.getValues()[12] << " " << calib->trafo_rgb_to_depth.calib.getValues()[13] << " " << calib->trafo_rgb_to_depth.calib.getValues()[14] << " " << calib->trafo_rgb_to_depth.calib.getValues()[15]<< "\n";

*/

	if(calib->disparityCalib.type==0){
		std::cout << "Disparity calib type KINECT" << "\n";
	}
	else{
		std::cout << "Disparity calib type AFFINE" << "\n";
	}
	std::cout << "Disparity calib x " << calib->disparityCalib.params.x << "\n";
	std::cout << "Disparity calib y " << calib->disparityCalib.params.y << "\n";

	mesh = NULL;
	if (createMeshingEngine) mesh = new ITMMesh(settings->deviceType == ITMLibSettings::DEVICE_CUDA ? MEMORYDEVICE_CUDA : MEMORYDEVICE_CPU);

	Vector2i trackedImageSize = ITMTrackingController::GetTrackedImageSize(settings, imgSize_rgb, imgSize_d);

	renderState_live = visualisationEngine->CreateRenderState(trackedImageSize);
	renderState_freeview = NULL; //will be created by the visualisation engine

	denseMapper = new ITMDenseMapper<ITMVoxel, ITMVoxelIndex>(settings);
	denseMapper->ResetScene(scene);

	imuCalibrator = new ITMIMUCalibrator_iPad();
	tracker = ITMTrackerFactory<ITMVoxel, ITMVoxelIndex>::Instance().Make(trackedImageSize, settings, lowLevelEngine, imuCalibrator, scene);
	trackingController = new ITMTrackingController(tracker, visualisationEngine, lowLevelEngine, settings);

	trackingState = trackingController->BuildTrackingState(trackedImageSize);
	tracker->UpdateInitialPose(trackingState);

	view = NULL; // will be allocated by the view builder

	fusionActive = true;
	mainProcessingActive = true;
}

ITMMainEngine::~ITMMainEngine()
{
	delete renderState_live;
	if (renderState_freeview!=NULL) delete renderState_freeview;

	delete scene;

	delete denseMapper;
	delete trackingController;

	delete tracker;
	delete imuCalibrator;

	delete lowLevelEngine;
	delete viewBuilder;

	delete trackingState;
	if (view != NULL) delete view;

	delete visualisationEngine;

	if (meshingEngine != NULL) delete meshingEngine;

	if (mesh != NULL) delete mesh;
}

ITMMesh* ITMMainEngine::UpdateMesh(void)
{
	if (mesh != NULL) meshingEngine->MeshScene(mesh, scene);
	return mesh;
}

void ITMMainEngine::SaveSceneToMesh(const char *objFileName)
{
	if (mesh == NULL) return;
	meshingEngine->MeshScene(mesh, scene);
	mesh->WriteSTL(objFileName);
}

void ITMMainEngine::ProcessFrame(ITMUChar4Image *rgbImage, ITMShortImage *rawDepthImage, ITMIMUMeasurement *imuMeasurement)
{
	Vector4u *dataPtr = rgbImage->GetData(MEMORYDEVICE_CPU);
	//printf("%d %d\n",rgbImage->noDims.x,rgbImage->noDims.y);

	short min = 1000.0;
	short max = 0.0;

	short *dataPtr2 = rawDepthImage->GetData(MEMORYDEVICE_CPU);
	//printf("%d %d\n",rawDepthImage->noDims.x,rawDepthImage->noDims.y);
	for(int i=0;i<rawDepthImage->noDims.x*rawDepthImage->noDims.y;i++){
		if(min > dataPtr2[i]) min = dataPtr2[i];
		if(max < dataPtr2[i]) max = dataPtr2[i];
	}
	//printf("min %d max %d\n",min,max);

	// prepare image and turn it into a depth image
	if (imuMeasurement==NULL) viewBuilder->UpdateView(&view, rgbImage, rawDepthImage, settings->useBilateralFilter,settings->modelSensorNoise);
	else viewBuilder->UpdateView(&view, rgbImage, rawDepthImage, settings->useBilateralFilter, imuMeasurement);

	if (!mainProcessingActive) return;

	// tracking
	trackingController->Track(trackingState, view);

	printf("Tracking-Pose ITM-ICP:\n");
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			printf("%.2f ", trackingState->pose_d->GetM().getValues()[i*4+j]);
		}
		printf("\n");
	}


	// fusion
	if (fusionActive) denseMapper->ProcessFrame(view, trackingState, scene, renderState_live);

	// raycast to renderState_live for tracking and free visualisation
	trackingController->Prepare(trackingState, view, renderState_live);
}

void ITMMainEngine::ProcessFrame(ITMUChar4Image *rgbImage, ITMShortImage *rawDepthImage, Matrix4f* icp)
{
	// prepare image and turn it into a depth image
	viewBuilder->UpdateView(&view, rgbImage, rawDepthImage, settings->useBilateralFilter,settings->modelSensorNoise);

	if (!mainProcessingActive) return;

	// tracking
	//trackingController->Track(trackingState, view);
	trackingState->requiresFullRendering = true;
	if (trackingState->age_pointCloud!=-1) trackingState->pose_d->SetM(*icp);

	printf("Tracking-Pose own ICP:\n");
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			printf("%.2f ", trackingState->pose_d->GetM().getValues()[i*4+j]);
		}
		printf("\n");
	}

	// fusion
	if (fusionActive) denseMapper->ProcessFrame(view, trackingState, scene, renderState_live);

	// raycast to renderState_live for tracking and free visualisation
	trackingController->Prepare(trackingState, view, renderState_live);
}

Vector2i ITMMainEngine::GetImageSize(void) const
{
	return renderState_live->raycastImage->noDims;
}

void ITMMainEngine::GetImage(ITMUChar4Image *out, GetImageType getImageType, ITMPose *pose, ITMIntrinsics *intrinsics)
{
	if (view == NULL) return;

	out->Clear();

	switch (getImageType)
	{
	case ITMMainEngine::InfiniTAM_IMAGE_ORIGINAL_RGB:
		out->ChangeDims(view->rgb->noDims);
		if (settings->deviceType == ITMLibSettings::DEVICE_CUDA)
			out->SetFrom(view->rgb, ORUtils::MemoryBlock<Vector4u>::CUDA_TO_CPU);
		else out->SetFrom(view->rgb, ORUtils::MemoryBlock<Vector4u>::CPU_TO_CPU);
		break;
	case ITMMainEngine::InfiniTAM_IMAGE_ORIGINAL_DEPTH:
		out->ChangeDims(view->depth->noDims);
		if (settings->trackerType==ITMLib::Objects::ITMLibSettings::TRACKER_WICP)
		{
			if (settings->deviceType == ITMLibSettings::DEVICE_CUDA) view->depthUncertainty->UpdateHostFromDevice();
			ITMVisualisationEngine<ITMVoxel, ITMVoxelIndex>::WeightToUchar4(out, view->depthUncertainty);
		}
		else
		{
			if (settings->deviceType == ITMLibSettings::DEVICE_CUDA) view->depth->UpdateHostFromDevice();
			ITMVisualisationEngine<ITMVoxel, ITMVoxelIndex>::DepthToUchar4(out, view->depth);
		}

		break;
	case ITMMainEngine::InfiniTAM_IMAGE_SCENERAYCAST:
	{
		ORUtils::Image<Vector4u> *srcImage = renderState_live->raycastImage;
		out->ChangeDims(srcImage->noDims);
		if (settings->deviceType == ITMLibSettings::DEVICE_CUDA)
			out->SetFrom(srcImage, ORUtils::MemoryBlock<Vector4u>::CUDA_TO_CPU);
		else out->SetFrom(srcImage, ORUtils::MemoryBlock<Vector4u>::CPU_TO_CPU);
		break;
	}
	case ITMMainEngine::InfiniTAM_IMAGE_FREECAMERA_SHADED:
	case ITMMainEngine::InfiniTAM_IMAGE_FREECAMERA_COLOUR_FROM_VOLUME:
	case ITMMainEngine::InfiniTAM_IMAGE_FREECAMERA_COLOUR_FROM_NORMAL:
	{
		IITMVisualisationEngine::RenderImageType type = IITMVisualisationEngine::RENDER_SHADED_GREYSCALE;
		if (getImageType == ITMMainEngine::InfiniTAM_IMAGE_FREECAMERA_COLOUR_FROM_VOLUME) type = IITMVisualisationEngine::RENDER_COLOUR_FROM_VOLUME;
		else if (getImageType == ITMMainEngine::InfiniTAM_IMAGE_FREECAMERA_COLOUR_FROM_NORMAL) type = IITMVisualisationEngine::RENDER_COLOUR_FROM_NORMAL;
		if (renderState_freeview == NULL) renderState_freeview = visualisationEngine->CreateRenderState(out->noDims);

		visualisationEngine->FindVisibleBlocks(pose, intrinsics, renderState_freeview);
		visualisationEngine->CreateExpectedDepths(pose, intrinsics, renderState_freeview);
		visualisationEngine->RenderImage(pose, intrinsics, renderState_freeview, renderState_freeview->raycastImage, type);

		if (settings->deviceType == ITMLibSettings::DEVICE_CUDA)
			out->SetFrom(renderState_freeview->raycastImage, ORUtils::MemoryBlock<Vector4u>::CUDA_TO_CPU);
		else out->SetFrom(renderState_freeview->raycastImage, ORUtils::MemoryBlock<Vector4u>::CPU_TO_CPU);
		break;
	}
	case ITMMainEngine::InfiniTAM_IMAGE_UNKNOWN:
		break;
	};
}

void ITMMainEngine::turnOnIntegration() { fusionActive = true; }
void ITMMainEngine::turnOffIntegration() { fusionActive = false; }
void ITMMainEngine::turnOnMainProcessing() { mainProcessingActive = true; }
void ITMMainEngine::turnOffMainProcessing() { mainProcessingActive = false; }
