//
//  WAVideoBox.h
//  WA
//
//  Created by 黄锐灏 on 17/9/10.
//  Copyright © 2017年 黄锐灏. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "WAAVSEGearboxCommand.h"
typedef NS_ENUM(NSUInteger,WAVideoExportRatio) {
    WAVideoExportRatioLowQuality,// 自动分辩率
    WAVideoExportRatioMediumQuality,// 自动分辩率
    WAVideoExportRatioHighQuality, // 自动分辩率
    WAVideoExportRatio640x480,
    WAVideoExportRatio960x540,
    WAVideoExportRatio1280x720
};

/*
 PS:为了灵活的调用、任意使用,效果与代码的调用顺序有关
 比如先变速，再换音轨，明显后面才换的音轨不会有变速效果
 
 WAVideoBox的工作区域:
    ----缓存区,appedVideo后缓存区域
    ----工作区,视频指令区域，只有在这区域的视频才是有效操作
    ----合成区,完成视频指令后待合成区域
 
 1､appendVideo:会将视频加入到缓存区，将工作区内容合成一个视频（无法再拆散）,并移到合成区，清空工作区
 2､视频操作指令:缓存区视频放到工作区，视频操作只对工作区视频有效
 3､commit:合成区域，将缓存区，合成区的视频移到工作区,视频操作对所有视频有效
 
 tip:线程安全，适用于短视频处理

*/

@interface WAVideoBox : NSObject

#pragma mark 输出设置
/**
 输出的视频分辨率,默认为960 * 540,需要在appendVideo前调用，若已经有视频在里面，设置无效
 */
@property (nonatomic , assign) WAVideoExportRatio ratio;

/**
 输出的视频质量(0~10)，默认为0(自动）,6差不多抖音视频质量平级，自动分辩率下无效
 */
@property (nonatomic , assign) NSInteger videoQuality;


#pragma mark 资源
/**
 拼接一个视频到缓存区
 @param videoPath 视频地址
 @return 加入状态
 */
- (BOOL)appendVideoByPath:(NSString *)videoPath;

/**
 拼接一个视频到缓存区
 @param videoAsset 视频资源
 @return 加入状态
 */
- (BOOL)appendVideoByAsset:(AVAsset *)videoAsset;

/**
 视频提交，将所有的视频提取到工作区，可以对所有视频操作，不可逆
 */
- (void)commit;

/**
 以下所有的视频操作会将缓存区的视频提取到工作区
 */
#pragma mark 操作指令
#pragma mark 裁取
/**
 视频截取
 @param range 时间范围，超出则为无效操作
 @return 操作状态
 */
- (BOOL)rangeVideoByTimeRange:(CMTimeRange)range;

/**
 视频旋转
 @param degress 角度，内部会调用%90,保证90度的倍数旋转
 @return 操作状态
 */
- (BOOL)rotateVideoByDegress:(NSInteger)degress;

#pragma mark 水印
/**
 为视频加入水印
 @param waterImg 图片
 @param relativeRect 相对尺寸 x ,y , width , height值均为 (0 ~ 1)
 @return 操作状态
 */
- (BOOL)appendWaterMark:(UIImage *)waterImg relativeRect:(CGRect)relativeRect;

#pragma mark 变速
/**
 视频按scale变速
 @param scale 速度倍率
 @return 操作状态
 */
- (BOOL)gearBoxWithScale:(CGFloat)scale;

/**
 按视频时间段变速
 @param scaleArray 速度倍率模型数组
 @return 操作状态
 */
- (BOOL)gearBoxTimeByScaleArray:(NSArray <WAAVSEGearboxCommandModel *> *)scaleArray;

#pragma mark 换音
- (BOOL)replaceSoundBySoundPath:(NSString *)soundPath;

#pragma mark 混音
- (BOOL)dubbedSoundBySoundPath:(NSString *)soundPath;

/**
 混音并调整原声立体声音量
 @param soundPath 声音地址
 @param volume 原声音量
 @param mixVolume 合声音量
 @param insetDuration 在哪里插入
 @return 操作状态
 */
- (BOOL)dubbedSoundBySoundPath:(NSString *)soundPath volume:(CGFloat)volume mixVolume:(CGFloat)mixVolume insertTime:(CGFloat)insetDuration;

#pragma mark 处理
/**
 异步视频处理
 @param complete 回调block
 */
- (void)asyncFinishEditByFilePath:(NSString *)filePath complete:(void (^)(NSError *error))complete;

/**
 同步视频处理
 @param complete 回调block
 */
- (void)syncFinishEditByFilePath:(NSString *)filePath complete:(void (^)(NSError *error))complete;

/**
 取消操作
 */
- (void)cancelEdit;

/**
 清空所有数据
 */
- (void)clean;

@end