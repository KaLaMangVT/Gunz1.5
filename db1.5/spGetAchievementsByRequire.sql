USE [GunzDB15]
GO
/****** Object:  StoredProcedure [dbo].[spGetAchievementsByRequire]    Script Date: 3/3/2567 13:19:37 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[spGetAchievementsByRequire]
@CID int,
@Type int,
@ID int
AS
	BEGIN
	
		SET NOCOUNT ON
		
		SELECT * FROM [dbo].[CharacterAchievements] WHERE CID = @CID and AchievementType = @Type and AchievementID = @ID
END


