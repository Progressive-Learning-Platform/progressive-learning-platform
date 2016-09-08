package edu.asu.plp.tool.prototype.view.menu.options.details;

import edu.asu.plp.tool.prototype.model.ApplicationSetting;
import edu.asu.plp.tool.prototype.model.SettingUtil;

/**
 * @author Nesbitt, Morgan on 2/27/2016.
 */
public class ProgrammerSettingDetails
{
	public static final ProgrammerSettingDetails DEFAULT = ProgrammerSettingDetails.defaultDetails();

	private String programInChunks;
	private String maximumChunkSize;
	private String receiveTimeoutMilliseconds;
	private String autoDetectSerialPorts;

	private ProgrammerSettingDetails()
	{

	}

	public ProgrammerSettingDetails( String programInChunks, String maximumChunkSize, String
			receiveTimeoutMilliseconds,
			String autoDetectSerialPorts )
	{
		this.programInChunks = programInChunks;
		this.maximumChunkSize = maximumChunkSize;
		this.receiveTimeoutMilliseconds = receiveTimeoutMilliseconds;
		this.autoDetectSerialPorts = autoDetectSerialPorts;
	}

	public ProgrammerSettingDetails( ProgrammerSettingDetails details )
	{
		this(details.programInChunks, details.maximumChunkSize, details.receiveTimeoutMilliseconds,
			 details.autoDetectSerialPorts);
	}

	private static ProgrammerSettingDetails defaultDetails()
	{
		ProgrammerSettingDetails details = new ProgrammerSettingDetails();

		ApplicationSetting setting = ApplicationSetting.PROGRAMMER_PROGRAM_IN_CHUNKS;
		details.programInChunks =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		setting = ApplicationSetting.PROGRAMMER_MAXIMUM_CHUNK_SIZE;
		details.maximumChunkSize =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		setting = ApplicationSetting.PROGRAMMER_TIMEOUT_MILLISECONDS;
		details.receiveTimeoutMilliseconds =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		setting = ApplicationSetting.PROGRAMMER_AUTO_DETECT_SERIAL_PORTS;
		details.autoDetectSerialPorts =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		return details;
	}

	public String getProgramInChunks()
	{
		return programInChunks;
	}

	public String getMaximumChunkSize()
	{
		return maximumChunkSize;
	}

	public String getReceiveTimeoutMilliseconds()
	{
		return receiveTimeoutMilliseconds;
	}

	public String getAutoDetectSerialPorts()
	{
		return autoDetectSerialPorts;
	}
}
